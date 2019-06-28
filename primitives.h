#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "renderer.h"

void drawPixel(const SDL_Surface* surface, int x, int y, Vec3 color);
void drawLine(const SDL_Surface* surface, int x0, int y0, int x1, int y1, Vec3 color);
void drawWireFrame(const SDL_Surface* surface, Vec4 v0, Vec4 v1, Vec4 v2, Vec3 color);
void drawTriangleHalfSpace(const SDL_Surface* surface, float* zBuffer, const Texture& texture, Vertex v0, Vertex v1, Vertex v2, Vec3 color);
void drawTriangleHalfSpaceFlat(RenderContext* context, Vec3 color, Vertex v0, Vertex v1, Vertex v2);

#endif