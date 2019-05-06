#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "renderer.h"
namespace primitives {
    void drawPixel(const SDL_Surface* surface, int x, int y, Vec4 color);

    void drawLine(const SDL_Surface* surface, int x0, int y0, int x1, int y1, Vec4 color);

    void drawWireFrame(const SDL_Surface* surface, const mat4x4& viewportTransform, Vec4 v0, Vec4 v1, Vec4 v2, Vec4 color);

    void drawTriangleHalfSpace(const SDL_Surface* surface, const mat4x4& viewportTransform,
        std::vector<float>& zBuffer,const Texture& texture, Vertex v0, Vertex v1, Vertex v2, Vec4 color);
}
#endif