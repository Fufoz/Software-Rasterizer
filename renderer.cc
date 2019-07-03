#include "renderer.h"
#include "primitives.h"
#include "input.h"
#include "camera.h"
#include "clipper.h"
#include <stdio.h>
#include <limits>

mat4x4 viewportTransform = {};
mat4x4 perspectiveTransform = {};
Vec4 clearColor = {};

void setRenderState(const mat4x4& viewport, const mat4x4 perspective, const Vec4& clear)
{
    viewportTransform  = viewport;
    perspectiveTransform = perspective;
    clearColor = clear;
}

bool windowClosed()
{
    return isKeyPressed(BTN_ESCAPE);
}

static void clearDepthBuffer(float* zBuffer, uint32_t width, uint32_t height)
{
    std::fill(zBuffer, zBuffer + width * height, std::numeric_limits<float>::max());
}

bool createSoftwareRenderer(RenderContext* context, const char* title, uint32_t width, uint32_t height)
{
  	SDL_Init(SDL_INIT_VIDEO);

  	SDL_Window *window = SDL_CreateWindow(
  	  title,
  	  SDL_WINDOWPOS_UNDEFINED,
  	  SDL_WINDOWPOS_UNDEFINED,
  	  width,
  	  height,
  	  SDL_WINDOW_RESIZABLE
  	);

    if(!window) {
        printf("Failed to create SDL_Window!\n");
        return false;
    }

    SDL_WarpMouseInWindow(window, width / 2, height / 2);
    SDL_ShowCursor(0);
    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
    if(!renderer) {
        printf("Failed to create SDL_Renderer!\n");
        return false;
    }

	SDL_Surface *surface = SDL_GetWindowSurface(window);

    context->window.window = window;
    context->surface = surface;
    context->window.width = width;
    context->window.height = height;

    float* ptr = (float*)malloc(width * height * sizeof(float));
    if(!ptr)
        return false;

    context->zBuffer = ptr;
    clearDepthBuffer(context->zBuffer, width, height);
    return true;
}


void destroySoftwareRenderer(RenderContext* context)
{
  	SDL_DestroyWindow(context->window.window);
  	SDL_Quit();
}

void beginFrame(RenderContext* context)
{
    //if window has been resized
    if(context->surface->w != context->window.width || context->surface->h != context->window.height) {
        context->window.width = context->surface->w;
        context->window.height = context->surface->h;
        free(context->zBuffer);
        float* ptr = (float*)malloc(context->window.width * context->window.height * sizeof(float));
        assert(ptr);
        context->zBuffer = ptr;
        viewportTransform = viewport(context->window.width, context->window.height);
    }

    clearDepthBuffer(context->zBuffer, context->window.width, context->window.height);

    SDL_FillRect(context->surface, NULL, SDL_MapRGBA(context->surface->format,
        clearColor.R,
        clearColor.G,
        clearColor.B,
        clearColor.A)
    );
}

static Triangle getTriangle(const Mesh& mesh, const Face& face)
{
    Triangle out = {};
    out.v1.pos  = homogenize(mesh.vertPos[face.vIndex[0] - 1]);
    out.v2.pos =  homogenize(mesh.vertPos[face.vIndex[1] - 1]);
    out.v3.pos  = homogenize(mesh.vertPos[face.vIndex[2] - 1]);

    out.v1.texCoords  = mesh.texCoord[face.tIndex[0] - 1];
    out.v2.texCoords =  mesh.texCoord[face.tIndex[1] - 1];
    out.v3.texCoords  = mesh.texCoord[face.tIndex[2] - 1];

    out.v1.normal  = mesh.normals[face.nIndex[0] - 1];
    out.v2.normal =  mesh.normals[face.nIndex[1] - 1];
    out.v3.normal  = mesh.normals[face.nIndex[2] - 1];

    return out;
}

void renderObject(RenderContext* context, const RenderObject& object, const Camera& camera)
{
    mat4x4 modelToWorldTransform = loadScale(object.transform.scale) * loadTranslation(object.transform.translate);

    for(uint32_t i = 0; i < object.mesh->faces.size(); i++) {

        Triangle input = getTriangle(*object.mesh, object.mesh->faces[i]);
        Triangle out = {};

        out.v1.pos = input.v1.pos * modelToWorldTransform * camera.worldToCameraTransform * perspectiveTransform;
        out.v2.pos = input.v2.pos * modelToWorldTransform * camera.worldToCameraTransform * perspectiveTransform;
        out.v3.pos = input.v3.pos * modelToWorldTransform * camera.worldToCameraTransform * perspectiveTransform;

        out.v1.texCoords = input.v1.texCoords;
        out.v2.texCoords = input.v2.texCoords;
        out.v3.texCoords = input.v3.texCoords;

        out.v1.normal = normaliseVec3(input.v1.normal * inverse(transpose(modelToWorldTransform)) );
        out.v2.normal = normaliseVec3(input.v2.normal * inverse(transpose(modelToWorldTransform)) );
        out.v3.normal = normaliseVec3(input.v3.normal * inverse(transpose(modelToWorldTransform)) );
        
        const Vec4& v1 = out.v1.pos;
        const Vec4& v2 = out.v2.pos;
        const Vec4& v3 = out.v3.pos;

        Vec3 firstFaceEdge =  v2.xyz - v1.xyz;
        Vec3 secondFaceEdge = v3.xyz - v1.xyz;
        Vec3 faceNormal = normaliseVec3(cross(firstFaceEdge, secondFaceEdge));
        Vec3 normalColor = {255.f, 0.f, 0.f};

        //the triangle is more lid the more it's normal is aligned with the light direction
        Vec3 cameraRay = normaliseVec3(camera.forward - v1.xyz);
        float diffuse = dotVec3(faceNormal, cameraRay);
        float ambient = 0.4f;

        //face culling
        if(diffuse <= 0.f) {
            diffuse = std::abs(diffuse);
            Vec3 lightDirection = {0.f, 0.f, -1.f};
            Vec3 renderColor = diffuse * object.flatColor;
            //Vec4 objectColor = {219.f, 112.f, 147.f, 255.f};
            
            
            //if the whole triangle inside the view frustum
            if( isInsideViewFrustum(v1) &&
                isInsideViewFrustum(v2) &&
                isInsideViewFrustum(v3)) {
                    switch(object.mode) {
                        case MODE_WIREFRAME:
                            drawWireFrame(context->surface, out.v1.pos, out.v2.pos, out.v3.pos, renderColor);
                            break;
                        case MODE_FLATCOLOR:
                            drawTriangleHalfSpaceFlat(context, renderColor, out.v1, out.v2, out.v3);
                            break;
                        case MODE_TEXTURED:
                            drawTriangleHalfSpace(context->surface,
                                context->zBuffer,
                                *object.texture, out.v1, out.v2, out.v3,
                                renderColor);
                            break;
                        default:
                            break;
                    }

            } else {//else clip polygon

                ClippResult result = clipTriangle(out.v1, out.v2, out.v3);

                for(size_t i = 0; i < result.numTriangles; i++) {
                    switch(object.mode) {
                        case MODE_WIREFRAME:
                            drawWireFrame(context->surface, result.triangles[i].v1.pos,
                                result.triangles[i].v2.pos,
                                result.triangles[i].v3.pos,
                                renderColor);
                            break;
                        case MODE_FLATCOLOR:
                            drawTriangleHalfSpaceFlat(context, renderColor,
                                result.triangles[i].v1,
                                result.triangles[i].v2,
                                result.triangles[i].v3
                            );
                            break;
                        case MODE_TEXTURED:
                            drawTriangleHalfSpace(context->surface,
                                context->zBuffer,
                                *object.texture,
                                result.triangles[i].v1,
                                result.triangles[i].v2,
                                result.triangles[i].v3,                                
                                renderColor);
                            break;
                        default:
                            break;
                    }
                }//for clip tris
            }//else
        }//diffuse check
    }//main face loop
}

void endFrame(RenderContext* context)
{
    SDL_UpdateWindowSurface(context->window.window);
}