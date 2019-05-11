#include "renderer.h"
#include "primitives.h"
#include <stdio.h>


Vec3 cameraUp = {0.f,1.f,0.f};
Vec3 cameraPos = {0.f,0.f,3.f};
Vec3 cameraForward = {0.f,0.f,-1.f};

void processInput(RenderContext* context)
{
    SDL_Event event;
	while(SDL_PollEvent(&event)) {
    	if (event.type == SDL_QUIT) {
		    context->isRunning = false;
    	}
        float speed = 0.05f;
    	if (event.type == SDL_KEYDOWN) {
            switch(event.key.keysym.sym) {
                case SDLK_ESCAPE :
                    context->isRunning = false;
                    break;
                case SDLK_w :
                    printf("w\n");
                    cameraPos += speed * cameraForward;
                    break;
                case SDLK_a :
                    printf("a\n");
                    cameraPos -= normaliseVec3(cross(cameraForward, cameraUp)) * speed;
                    break;
                case SDLK_s :
                    printf("s\n");
                    cameraPos -= speed * cameraForward;
                    break;
                case SDLK_d :
                    printf("d\n");
                    cameraPos += normaliseVec3(cross(cameraForward, cameraUp)) * speed;
                    break;
          }
        }
	}
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

    mat4x4 viewportTransform = viewport(context->width, context->height);
    mat4x4 rotation = rotateX(x) * rotateY(x);
    x+=0.1;
    mat4x4 modelWorldTransform = loadScale(Vec3{0.5f, 0.5f, 0.5f}) * rotation;
    mat4x4 worldCameraTransform = lookAt(cameraPos, cameraPos + cameraForward);    
    mat4x4 perspective = perspectiveProjection(90.f, context->width / context->height, 0.1f, 100.f);
    
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
//            primitives::drawTriangleHalfSpace(context->surface, viewportTransform, context->zBuffer,
//                target.texture, Vertex{v1,Vec3{}}, Vertex{v2,Vec3{}}, Vertex{v3,Vec3{}}, Vec4{98.f, 155.f, 123.f, 255.f});
        Vec4 pinkColor = {219.f, 112.f, 147.f, 255.f};
        if(intensity > 0)
            primitives::drawTriangleHalfSpace(context->surface, viewportTransform, context->zBuffer,
                target.texture, Vertex{v1,Vec3{}}, Vertex{v2,Vec3{}}, Vertex{v3,Vec3{}}, Vec4{intensity * pinkColor.R, intensity * pinkColor.G, intensity * pinkColor.B, pinkColor.A});

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