#include "renderer.h"
#include "primitives.h"
#include "input.h"
#include "camera.h"
#include "clipper.h"
#include <stdio.h>
#include <limits>

namespace globals {
    mat4x4 viewportTransform;
    mat4x4 perspectiveTransform;
    Vec4 clearColor;
    Camera camera;
};


void setViewPort(const mat4x4& viewPort)
{
    globals::viewportTransform = viewPort;
}

void setCamera(Camera& cam)
{
    globals::camera = cam;
}

void setClearColor(const Vec4& color)
{
    globals::clearColor = color;
}

void setPerspective(const mat4x4& perspective)
{
    globals::perspectiveTransform = perspective;
}

bool windowClosed()
{
    return isKeyPressed(BTN_ESCAPE);
}

static void clearDepthBuffer(std::vector<float>& zBuffer, uint32_t width, uint32_t height)
{
    std::fill(zBuffer.begin(), zBuffer.end(), std::numeric_limits<float>::max());
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
    context->zBuffer.resize(width * height);
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
    if(context->surface->w != context->window.width || context->surface->h != context->window.height){
        context->window.width = context->surface->w;
        context->window.height = context->surface->h;
        context->zBuffer.resize(context->window.width * context->window.height);
    }
    clearDepthBuffer(context->zBuffer, context->window.width, context->window.height);
    SDL_FillRect(context->surface, NULL, SDL_MapRGBA(context->surface->format,
        globals::clearColor.R,
        globals::clearColor.G,
        globals::clearColor.B,
        globals::clearColor.A)
    );

    updateCameraPosition(&globals::camera);
}



void renderObject(RenderContext* context, const RenderObject& object, RenderMode renderMode)
{
    mat4x4 modelToWorldTransform = loadScale(object.transform.scale) * loadTranslation(object.transform.translate);
    
    for(uint32_t i = 0; i < object.mesh->faces.size(); i++) {
        
        VertexCoords faceCoords = grabTriVertexCoord(*object.mesh, object.mesh->faces[i]);
        Vec4 v1 = homogenize(faceCoords.first)  * modelToWorldTransform * globals::camera.worldToCameraTransform * globals::perspectiveTransform;
        Vec4 v2 = homogenize(faceCoords.second) * modelToWorldTransform * globals::camera.worldToCameraTransform * globals::perspectiveTransform;
        Vec4 v3 = homogenize(faceCoords.third)  * modelToWorldTransform * globals::camera.worldToCameraTransform * globals::perspectiveTransform;
        
        Vec3 firstFaceEdge =  v2.xyz - v1.xyz;
        Vec3 secondFaceEdge = v3.xyz - v1.xyz;
        Vec3 faceNormal = normaliseVec3(cross(firstFaceEdge, secondFaceEdge));
        
        //the triangle is more lid the more it's normal is aligned with the light direction
        Vec3 cameraRay = normaliseVec3(globals::camera.forward - v1.xyz);
        float intensity = dotVec3(faceNormal, cameraRay);
        
        if(intensity < 0.f) {
            Vec4 pinkColor = {219.f, 112.f, 147.f, 255.f};
            Vec4 renderColor = {-intensity * pinkColor.R, -intensity * pinkColor.G, -intensity * pinkColor.B, pinkColor.A};

            float doubletriArea = computeArea(v1.xyz, v2.xyz, v2.xyz);
            //backface culling
            if(doubletriArea < 0)
                continue;
            
            //if the whole triangle inside the view frustum
            if( clipper::isInsideViewFrustum(v1) &&
                clipper::isInsideViewFrustum(v2) &&
                clipper::isInsideViewFrustum(v3)) {

                if(renderMode & MODE_WIREFRAME) {
                    primitives::drawWireFrame(context->surface, 
                    globals::viewportTransform, v1, v2, v3,
                    renderColor);
                }
                if(renderMode & MODE_TEXTURED)
                primitives::drawTriangleHalfSpace(context->surface,
                    globals::viewportTransform, context->zBuffer,
                    *object.texture,
                    Vertex{v1, Vec3{}},
                    Vertex{v2, Vec3{}},
                    Vertex{v3, Vec3{}},
                    renderColor);

            } else {//else clip polygon

                //v0 = {2.f, -0.5f, 0.f, 1.f};
                //v1 = {2.f, -0.5f, 0.f, 1.f};
                //v2 = {0.f, 1.3f, 0.f, 1.f};
                clipper::ClippResult result = clipper::clipTriangle(v1, v2, v3);
                for(size_t i = 0; i < result.numTriangles; i++)
                    primitives::drawTriangleHalfSpace(context->surface, globals::viewportTransform, context->zBuffer, *object.texture,
                        Vertex{result.triangles[i].v1, Vec3{}},
                        Vertex{result.triangles[i].v2, Vec3{}},
                        Vertex{result.triangles[i].v3, Vec3{}}, renderColor);
            }
        }
    }
}

void endFrame(RenderContext* context)
{
    SDL_UpdateWindowSurface(context->window.window);
}