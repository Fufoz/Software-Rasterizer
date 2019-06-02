#include "renderer.h"
#include "primitives.h"
#include "input.h"
#include <stdio.h>
#include "camera.h"

bool windowClosed()
{
    return isKeyPressed(BTN_ESCAPE);
}

static void clearDepthBuffer(std::vector<float>& zBuffer, uint32_t width, uint32_t height)
{
    std::fill(zBuffer.begin(), zBuffer.end(), -std::numeric_limits<float>::max());
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
    context->window.isRunning = true;
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
    SDL_FillRect(context->surface, NULL, SDL_MapRGBA(context->surface->format, 0, 0, 0, 255));
}

Camera camera {Vec3{0.f, 0.f, 3.f}, Vec3{0.f, 0.f, -1.f}, Vec3{0.f, 1.f, 0.f}, PROJ_PERSPECTIVE};

void commitFrame(RenderContext* context, const Target& target)
{
    
    mat4x4 viewportTransform = viewport(context->window.width, context->window.height);

    mat4x4 modelWorldTransform = loadScale(Vec3{0.5f, 0.5f, 0.5f});
    updateCameraPosition(&camera);
    
    mat4x4 worldCameraTransform = lookAt(camera.camPos, camera.camPos + camera.forward);    

    mat4x4 perspective = perspectiveProjection(90.f, context->window.width / context->window.height, 0.1f, 100.f);
    
    //the triangle is more lid the more it's normal is aligned with the light direction
    Vec3 lightDirection = {0, 0, 1.f};

    for(uint32_t i = 0; i < target.mesh.faces.size(); i++) {
        VertexCoords faceCoords =  grabTriVertexCoord(target.mesh, target.mesh.faces[i]);
        Vec4 v1 = homogenize(faceCoords.first)  * modelWorldTransform * worldCameraTransform * perspective;
        Vec4 v2 = homogenize(faceCoords.second) * modelWorldTransform * worldCameraTransform * perspective;
        Vec4 v3 = homogenize(faceCoords.third)  * modelWorldTransform * worldCameraTransform * perspective;

        Vec3 firstFaceEdge =  v2.xyz - v1.xyz;
        Vec3 secondFaceEdge = v3.xyz - v1.xyz;
        Vec3 faceNormal = normaliseVec3(cross(firstFaceEdge, secondFaceEdge));
        float intensity = dotVec3(faceNormal, lightDirection);
        Vec4 pinkColor = {219.f, 112.f, 147.f, 255.f};
        
        if(intensity > 0) {
            pipeline::renderTriangle(context->surface, viewportTransform, context->zBuffer,
                target.texture, Vertex{v1,Vec3{}}, Vertex{v2,Vec3{}}, Vertex{v3,Vec3{}}, Vec4{intensity * pinkColor.R, intensity * pinkColor.G, intensity * pinkColor.B, pinkColor.A});
        }
    }
}

void endFrame(RenderContext* context)
{
    SDL_UpdateWindowSurface(context->window.window);
}