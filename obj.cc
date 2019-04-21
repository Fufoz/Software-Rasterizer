#include "obj.h"

bool load(const char* model, ObjModel& data)
{
    FILE* mesh = fopen(model, "rb");
    if(!mesh) {
        return false;
    }

    bool hasTextureCoords = false;
    bool hasNormals = false;

    Vec3 vertexPosition = {};
    Vec3 textCoord = {};
    Vec3 normals = {};

    char buff[256];
    
    while(true) {
        int err = fscanf(mesh, "%s",buff);
        if(err == EOF)
            break;
        if(!strcmp(buff,"v")) {

            err = fscanf(mesh, "%f %f %f",
                &vertexPosition.x, &vertexPosition.y, &vertexPosition.z);
            if(err != 3) {
                printf("PARSE ERROR: failed to read vertex position!\n");
                return false;
            }
            data.vertPos.push_back(vertexPosition);

        }else if(!strcmp(buff,"vt")){
            
            err = fscanf(mesh, "%f %f %f",
                &textCoord.x, &textCoord.y, &textCoord.z);
            if(err != 3) {
                printf("PARSE ERROR: failed to read texture Coord position!\n");
                return false;
            }
            data.texCoord.push_back(textCoord);

        }else if(!strcmp(buff,"vn")){
            
            err = fscanf(mesh, "%f %f %f",
                &normals.x, &normals.y, &normals.z);
            if(err != 3) {
                printf("PARSE ERROR: failed to read normals position!\n");
                return false;
            }
            data.normals.push_back(normals);
            
        }else if(!strcmp(buff,"f")) {
            Face face = {};
            int vertexIndex1 = 0;
            int vertexIndex2 = 0;
            int vertexIndex3 = 0;
            
            int textureCoord1 = 0;
            int textureCoord2 = 0;
            int textureCoord3 = 0;
            
            int normals1 = 0;
            int normals2 = 0;
            int normals3 = 0;
            
            err = fscanf(mesh, "%d/%d/%d %d/%d/%d %d/%d/%d",
            &vertexIndex1, &textureCoord1, &normals1,
            &vertexIndex2, &textureCoord2, &normals2,
            &vertexIndex3, &textureCoord3, &normals3);
            if(err == 9) {
                face.vIndex[0] = vertexIndex1;
                face.vIndex[1] = vertexIndex2;
                face.vIndex[2] = vertexIndex3;

                face.tIndex[0] = textureCoord1;
                face.tIndex[1] = textureCoord2;
                face.tIndex[2] = textureCoord3;

                face.nIndex[0] = normals1;
                face.nIndex[1] = normals2;
                face.nIndex[2] = normals3;

                data.faces.push_back(face);
                continue;
            }

            err = fscanf(mesh, "%d//%d %d//%d %d//%d",
            &vertexIndex1, &normals1,
            &vertexIndex2, &normals2,
            &vertexIndex3, &normals3);
            if(err == 6) {
                face.vIndex[0] = vertexIndex1;
                face.vIndex[1] = vertexIndex2;
                face.vIndex[2] = vertexIndex3;
                                
                face.nIndex[0] = normals1;
                face.nIndex[1] = normals2;
                face.nIndex[2] = normals3;

                data.faces.push_back(face);
                continue;
            }

            err = fscanf(mesh, "%d %d %d",
            &vertexIndex1, &vertexIndex2, &vertexIndex3);
            if(err == 3) {
                face.vIndex[0] = vertexIndex1;
                face.vIndex[1] = vertexIndex2;
                face.vIndex[2] = vertexIndex3;

                data.faces.push_back(face);
                continue;
            }
            printf("PARSE ERROR: FAILED TO PARSE FACES!\n");
            return false;
        }
    }

    printf("MESH INFO:\n Faces = %d\n VertexPositions = %d\n Normals = %d\n Texture Coords = %d\n",
    data.faces.size(), data.vertPos.size(), data.normals.size(), data.texCoord.size());

    return true;
}


VertexCoords grabTriVertexCoord(const ObjModel& model, const Face& face)
{
    VertexCoords out = {};
    out.first  = model.vertPos[face.vIndex[0] - 1];
    out.second = model.vertPos[face.vIndex[1] - 1];
    out.third  = model.vertPos[face.vIndex[2] - 1];
    return out;
}

TriangleTextCoords grabTriTextCoord(const ObjModel& model, const Face& face)
{
    TriangleTextCoords out = {};
    out.first  = model.texCoord[face.tIndex[0] - 1];
    out.second = model.texCoord[face.tIndex[1] - 1];
    out.third  = model.texCoord[face.tIndex[2] - 1];
    return out;
}
