#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include <stdio.h>
#include <cassert>

#include "vec.h"
struct alignas(16) Vertex
{
    Vec4 pos;
    Vec3 texCoords;
};

struct Face
{
    size_t vIndex[3];
    size_t tIndex[3];
    size_t nIndex[3];
};

struct ObjModel
{
    std::vector<Face> faces;
    std::vector<Vec3> vertPos;
    std::vector<Vec3> normals;
    std::vector<Vec3> texCoord;
};

struct VertexCoords
{
    Vec3 first;
    Vec3 second;
    Vec3 third;
};

struct TriangleTextCoords
{
    Vec3 first;
    Vec3 second;
    Vec3 third;
};

bool load(const char* model, ObjModel& data);
VertexCoords grabTriVertexCoord(const ObjModel& model, const Face& face);
TriangleTextCoords grabTriTextCoord(const ObjModel& model, const Face& face);

#endif