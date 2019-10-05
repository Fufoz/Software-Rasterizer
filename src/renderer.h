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

struct Transform
{
	Quat rotate;
	Vec3 scale;
	Vec3 translate;
};

struct RenderObject
{
	Mesh* mesh;
	Texture* texture;
	Texture* normalMap;
	Texture* heightMap;
	Transform transform;
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