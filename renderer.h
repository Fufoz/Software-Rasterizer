#ifndef SOFTWARE_RENDERER_H
#define SOFTWARE_RENDERER_H

#include <SDL2/SDL.h>
#include <vector>
#include "obj.h"
#include "math.h"
#include "texture.h"
#include "camera.h"

struct Window
{
    SDL_Window* window;
    int width;
    int height;
};

struct RenderContext
{
    float* zBuffer;
    SDL_Surface* surface;
    Window window;
};

struct Transform
{
    Quat rotate;
    Vec3 scale;
    Vec3 translate;
};

enum RenderMode
{
    MODE_WIREFRAME = 1 << 0,
    MODE_DEPTH     = 1 << 1,
    MODE_FLATCOLOR = 1 << 2,
    MODE_TEXTURED  = 1 << 3
};

struct RenderObject
{
    Mesh* mesh;
    Texture* texture;
    Transform transform;
    RenderMode mode;
    Vec3 flatColor = {1.f, 1.f, 1.f};
};

struct Vertex
{
    Vec4 pos;
    Vec3 texCoords;
    Vec3 normal;
    Vec3 color;
};

struct Triangle
{
    Vertex v1;
    Vertex v2;
    Vertex v3;
};

//render State
extern mat4x4 viewportTransform;
extern mat4x4 perspectiveTransform;
extern Vec4 clearColor;

bool windowClosed();

void setRenderState(const mat4x4& viewport, const mat4x4 perspective, const Vec4& clear);

bool createSoftwareRenderer(RenderContext* context, const char* title, uint32_t width, uint32_t height);

void destroySoftwareRenderer(RenderContext* context);

void processInput(RenderContext* context);

void beginFrame(RenderContext* context);

void renderObject(RenderContext* context, const RenderObject& object, const Camera& camera);

void endFrame(RenderContext* context);

#endif