#ifndef CLIPPER_H
#define CLIPPER_H
#include "maths.h"
#include "renderer.h"

#define MAX_CLIPPED_TRIANGLE_COUNT 5
#define MAX_CLIPPED_VERTEX_COUNT 7

struct ClippResult
{
    Triangle triangles[MAX_CLIPPED_TRIANGLE_COUNT];
    size_t numTriangles;//num vertices - 2
};

bool isInsideViewFrustum(const Vec4& pos);

ClippResult clipTriangle(const Vertex& v1, const Vertex& v2, const Vertex& v3);

#endif