#ifndef OBJ_H
#define OBJ_H

#include <vector>
#include <stdio.h>
#include <cassert>

#include "math.h"

struct Face
{
    size_t vIndex[3];
    size_t tIndex[3];
    size_t nIndex[3];
};

struct Mesh
{
    std::vector<Face> faces;
    std::vector<Vec3> vertPos;
    std::vector<Vec3> normals;
    std::vector<Vec3> texCoord;
};


bool loadMesh(const char* model, Mesh* data);


#endif