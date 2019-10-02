#ifndef SOFTWARE_RENDERER_H
#define SOFTWARE_RENDERER_H

#include <SDL2/SDL.h>
#include <vector>
#include "obj.h"
#include "maths.h"
#include "texture.h"
#include "camera.h"
#include "shaders.h"

struct Window
{
    SDL_Window* window;
    int width;
    int height;
};

struct RenderTargets
{
    float* zBuffer;
    Vec3* cBuffer;
};

struct RenderContext
{
    Window window;
    RenderTargets rtargets;
    SDL_Surface* surface;
};

//4xmsaa
static const uint8_t sampleCount = 4;
static const int8_t sampleLocX[sampleCount] = {6, -2, -6, 2};
static const int8_t sampleLocY[sampleCount] = {2, 6, -2, -6};

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
    Texture* normalMap;
    Texture* heightMap;
    Transform transform;
    RenderMode mode;
    Vec3 flatColor = {1.f, 1.f, 1.f};
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

void renderObject(RenderContext* context, const RenderObject& object, const Camera& camera, Shader& shader);

void endFrame(RenderContext* context);

#endif