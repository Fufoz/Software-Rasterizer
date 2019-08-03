#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include <stdio.h>
#include <cassert>

#include "math.h"

struct Face
{
    int64_t vIndex[3];
    int64_t tIndex[3];
    int64_t nIndex[3];
    int64_t tanIndex[3];
};

struct Mesh
{
    std::vector<Face> faces;
    std::vector<Vec3> vertPos;
    std::vector<Vec3> normals;
    std::vector<Vec3> texCoord;
    std::vector<Vec3> tangents;
};



bool loadMesh(const char* model, Mesh* data);
void averageNormals(Mesh* mesh);
void fillTangent(Mesh* mesh);
#endif