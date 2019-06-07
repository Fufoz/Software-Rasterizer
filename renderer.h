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
};

struct RenderContext
{
    std::vector<float> zBuffer;
    SDL_Surface* surface;
    Window window;
};

struct Transform
{
    Vec3 scale;
    Vec3 rotate;
    Vec3 translate;
};

struct RenderObject
{
    Mesh mesh;
    Texture texture;
    Transform transform;
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

void setViewPort(const mat4x4& viewPort);

void setClearColor(const Vec4& color);

bool createSoftwareRenderer(RenderContext* context, const char* title, uint32_t width, uint32_t height);

void destroySoftwareRenderer(RenderContext* context);

void beginFrame(RenderContext* context);

void processInput(RenderContext* context);

void commitFrame(RenderContext* context, const Target& target);

void renderObject(RenderContext* context, const RenderObject& object, RenderMode renderMode);

void endFrame(RenderContext* context);

#endif