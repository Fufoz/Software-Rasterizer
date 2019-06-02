#ifndef SOFTWARE_RENDERER_H
#define SOFTWARE_RENDERER_H

#include <SDL2/SDL.h>
#include <vector>
#include "obj.h"
#include "math.h"
#include "texture.h"

struct Window
{
    SDL_Window* window;
    uint32_t width;
    uint32_t height;
    const char* title;
    bool isRunning;
};

struct RenderContext
{
    std::vector<float> zBuffer;
    SDL_Surface* surface;
    Window window;
};

enum RenderMode
{
    MODE_WIREFRAME =  1 << 0,
    MODE_DEPTH     =  1 << 1,
    MODE_TEXTURED  =  1 << 2
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

bool windowClosed();

bool createSoftwareRenderer(RenderContext* context, const char* title, uint32_t width, uint32_t height);

void destroySoftwareRenderer(RenderContext* context);

void beginFrame(RenderContext* context);

void processInput(RenderContext* context);

/*only works with single mesh instance!!!*/
void commitFrame(RenderContext* context, const Target& target);

void endFrame(RenderContext* context);

#endif