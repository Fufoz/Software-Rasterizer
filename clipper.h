#ifndef CLIPPER_H
#define CLIPPER_H
#include "math.h"

namespace clipper {

#define MAX_CLIPPED_TRIANGLE_COUNT 5
#define MAX_CLIPPED_VERTEX_COUNT 7

struct Triangle
{
    Vec4 v1;
    Vec4 v2;
    Vec4 v3;
};

struct ClippResult
{
    Triangle triangles[MAX_CLIPPED_TRIANGLE_COUNT];
    size_t numTriangles;//num vertices - 2
};

bool isInsideViewFrustum(const Vec4& pos);

ClippResult clipTriangle(const Vec4& v1, const Vec4& v2, const Vec4& v3);

}//!clipper
#endif