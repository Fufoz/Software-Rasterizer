#include "renderer.h"
#include "primitives.h"
#include <stdio.h>

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

    context->window = window;
    context->renderer = renderer;
    context->surface = surface;
    context->width = width;
    context->height = height;
    context->isRunning = true;
    context->zBuffer.resize(width * height);
    clearDepthBuffer(context->zBuffer, width, height);
    return true;
}


void destroySoftwareRenderer(RenderContext* context)
{
  	SDL_DestroyWindow(context->window);
  	SDL_Quit();
}

void beginFrame(RenderContext* context)
{
    if(context->surface->w != context->width || context->surface->h != context->height){
        context->width = context->surface->w;
        context->height = context->surface->h;
        context->zBuffer.resize(context->width * context->height);
    }
    clearDepthBuffer(context->zBuffer, context->width, context->height);
    SDL_FillRect(context->surface, NULL, SDL_MapRGBA(context->surface->format, 0, 0, 0, 255));
}

float x = 1;

void commitFrame(RenderContext* context, const Target& target)
{
    //Vec4 v1 = {-1.f, -1.f, 0.f, 1.f};
    //Vec4 v2 = {1.f, -1.f, 0.f, 1.f};
    //Vec4 v3 = {0.f, 1.f, 0.f, 1.f};
    mat4x4 viewportTransform = viewport(context->width, context->height);
    mat4x4 rotation = rotateX(x) * rotateY(x);
    x+=1;
    mat4x4 modelWorldTransform = loadScale(Vec3{0.5f, 0.5f, 0.5f}) * rotation;
    viewportTransform = viewportTransform;
    
    Vec4 lightDirection = {0, 0, -1.f};
    
    for(uint32_t i = 0; i < target.mesh.faces.size(); i++) {
        VertexCoords faceCoords =  grabTriVertexCoord(target.mesh, target.mesh.faces[i]);
        Vec4 v1 = homogenize(faceCoords.first)  * modelWorldTransform;
        Vec4 v2 = homogenize(faceCoords.second) * modelWorldTransform;
        Vec4 v3 = homogenize(faceCoords.third)  * modelWorldTransform;

        Vec3 firstFaceEdge =  v2.xyz - v1.xyz;
        Vec3 secondFaceEdge = v3.xyz - v1.xyz;
        Vec3 faceNormal = normaliseVec3(cross(firstFaceEdge, secondFaceEdge));
        float intensity = dotVec3(lightDirection.xyz, faceNormal);

        //primitives::drawWireFrame(context->surface,viewportTransform,v1,v2,v3,Vec4{225.f, 249.f, 9.f,255.f });
            if(intensity > 0)
                primitives::drawTriangleHalfSpace(context->surface,viewportTransform,context->zBuffer,
                    target.texture,Vertex{v1,Vec3{}},Vertex{v2,Vec3{}},Vertex{v3,Vec3{}},Vec4{intensity * 255.f,intensity * 255.f, intensity * 255.f, 255.f});
    }
}

void endFrame(RenderContext* context)
{
    SDL_UpdateWindowSurface(context->window);
}

















#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
bool loadTexture(const char* path, Texture* out, bool flipImage)
{
    int twidth;
    int theight;
    int numChannels;

    stbi_set_flip_vertically_on_load(flipImage);
    uint8_t* data = stbi_load(path, &twidth, &theight, &numChannels, STBI_rgb);
    
    if(!data) {
        printf("Failed to load texture from path %s!\n", path);
        return false;
    }

    out->width =  twidth;
    out->height = theight;
    out->numc = numChannels;
    out->data = data;
    return true; 
}