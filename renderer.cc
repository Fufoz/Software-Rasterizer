#include "renderer.h"
#include "primitives.h"
#include "input.h"
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

    context->rtargets.zBuffer = ptr;
    clearDepthBuffer(context->rtargets.zBuffer, width, height);
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
        free(context->rtargets.zBuffer);
        float* ptr = (float*)malloc(context->window.width * context->window.height * sizeof(float));
        assert(ptr);
        context->rtargets.zBuffer = ptr;
        viewportTransform = viewport(context->window.width, context->window.height);
    }

    clearDepthBuffer(context->rtargets.zBuffer, context->window.width, context->window.height);

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
	
	if(mesh.meshFeatureMask & FEATURE_UVS) {
	    out.v1.texCoords  = mesh.texCoord[face.tIndex[0] - 1];
	    out.v2.texCoords =  mesh.texCoord[face.tIndex[1] - 1];
	    out.v3.texCoords  = mesh.texCoord[face.tIndex[2] - 1];
	}

	if(mesh.meshFeatureMask & FEATURE_NORMALS) {
	    out.v1.normal  = mesh.normals[face.nIndex[0] - 1];
	    out.v2.normal =  mesh.normals[face.nIndex[1] - 1];
	    out.v3.normal  = mesh.normals[face.nIndex[2] - 1];
	}

	if(mesh.meshFeatureMask & FEATURE_TANGENTS) {
	    out.v1.tangent  = mesh.tangents[face.tanIndex[0] - 1];
	    out.v2.tangent =  mesh.tangents[face.tanIndex[1] - 1];
	    out.v3.tangent  = mesh.tangents[face.tanIndex[2] - 1];
	}
	
    return out;
}

void renderObject(RenderContext* context, const RenderObject& object, const Camera& camera, Shader& shader)
{
    mat4x4 modelToWorldTransform =  loadScale(object.transform.scale) * loadTranslation(object.transform.translate);
    mat4x4 VP = camera.worldToCameraTransform * perspectiveTransform;
    mat4x4 normalTransform = inverse(transpose(modelToWorldTransform));

    shader.uniforms.in_VP = VP;
    shader.uniforms.in_normalTransform = normalTransform;
    shader.uniforms.in_cameraPosition = camera.camPos;

    for(uint32_t i = 0; i < object.mesh->faces.size(); i++) {

        Triangle input = getTriangle(*object.mesh, object.mesh->faces[i]);
        Triangle out = input;

        out.v1.pos = input.v1.pos * modelToWorldTransform;
        out.v2.pos = input.v2.pos * modelToWorldTransform;
        out.v3.pos = input.v3.pos * modelToWorldTransform;

        Vec4& v1 = out.v1.pos;
        Vec4& v2 = out.v2.pos;
        Vec4& v3 = out.v3.pos;

        Vec3 firstFaceEdge =  v2.xyz - v1.xyz;
        Vec3 secondFaceEdge = v3.xyz - v1.xyz;
        Vec3 faceNormal = normaliseVec3(cross(firstFaceEdge, secondFaceEdge));

        Vec3 centroid = (v1.xyz + v2.xyz + v3.xyz) * 0.333f;
        //the triangle is more lid the more it's normal is aligned with the light direction
        Vec3 cameraRay = normaliseVec3(camera.camPos - centroid);
        float lightIntensity = dotVec3(cameraRay, faceNormal);

        //backface culling
        if(lightIntensity >= 0.f) {
            ////////////////////////////////////////
            shader.uniforms.in_lightIntensity = lightIntensity;
            shader.uniforms.in_centerView = cameraRay;

            out.v1.texCoords = input.v1.texCoords;
            out.v2.texCoords = input.v2.texCoords;
            out.v3.texCoords = input.v3.texCoords;

            out.v1 = shader.vertexShader(out.v1, 0);
            out.v2 = shader.vertexShader(out.v2, 1);
            out.v3 = shader.vertexShader(out.v3, 2);

            //if the whole triangle inside the view frustum
            if( isInsideViewFrustum(out.v1.pos) &&
                isInsideViewFrustum(out.v2.pos) &&
                isInsideViewFrustum(out.v3.pos)) {
                drawTriangleHalfSpaceFlat(context, out.v1, out.v2, out.v3, shader);
            } else {//else clip polygon
                ClippResult result = clipTriangle(out.v1, out.v2, out.v3);
                for(size_t i = 0; i < result.numTriangles; i++) {
					Triangle& triangle = result.triangles[i];
                    //Ugliest durtiest hack to keep shader interpolants correct after clipping
                    triangle.v1.pos *= inverse(VP);
                    triangle.v2.pos *= inverse(VP);
                    triangle.v3.pos *= inverse(VP);
                    triangle.v1 = shader.vertexShader(triangle.v1, 0);
                    triangle.v2 = shader.vertexShader(triangle.v2, 1);
                    triangle.v3 = shader.vertexShader(triangle.v3, 2);
                    drawTriangleHalfSpaceFlat(context, triangle.v1, triangle.v2, triangle.v3, shader);
                }
            }
        }//backface cull
    }//main face loop
}


void endFrame(RenderContext* context)
{
    SDL_UpdateWindowSurface(context->window.window);
}