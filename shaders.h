#ifndef SHADER_H
#define SHADER_H
#include "renderer.h"

struct Shader
{
    Vertex vertexShader(const Vertex& in);
    Vec3 fragmentShader(const Vec2& pixelPos);
};

#endif