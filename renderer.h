#ifndef SOFTWARE_RENDERER_H
#define SOFTWARE_RENDERER_H

#include <SDL2/SDL.h>
#include <vector>
#include "obj.h"
#include "mat.h"

struct RenderContext
{
    std::vector<float> zBuffer;
    SDL_Renderer* renderer;
    SDL_Surface* surface;
    SDL_Window* window;
    uint32_t width;
    uint32_t height;
    bool isWindowChanged;
    bool isRunning;
};

struct Texture
{
    size_t width;
    size_t height;
    uint8_t numc;
    uint8_t* data;
};

struct Target
{
    Mesh mesh;
    Texture texture;
};

struct Vertex
{
    Vec4 pos;
    Vec3 texCoords;
};

bool createSoftwareRenderer(RenderContext* context, const char* title, uint32_t width, uint32_t height);

void destroySoftwareRenderer(RenderContext* context);

void beginFrame(RenderContext* context);

void processInput(RenderContext* context);

/*only works with single mesh instance!!!*/
void commitFrame(RenderContext* context, const Target& target);

void endFrame(RenderContext* context);

bool loadTexture(const char* path, Texture* out, bool flipImage = false);
#endif