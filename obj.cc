#include "obj.h"

static inline bool isVertexCoordinates(const char* buffer)
{
    return buffer[0] == 'v' && buffer[1] == ' ';
}

static inline bool isTextureCoordinates(const char* buffer)
{
    return buffer[0] == 'v' && buffer[1] == 't';
}

static inline bool isVertexNormals(const char* buffer)
{
    return buffer[0] == 'v' && buffer[1] == 'n';
}

static inline bool isFaces(const char* buffer)
{
    return buffer[0] == 'f' && buffer[1] == ' '; 
}

static float parceFloat(char** buffer)
{
    char* buff = *buffer;
    int buffOffset = 0;
    while(buff[buffOffset] && buff[buffOffset] != ' ') {
        buffOffset++;
    }
    buff[buffOffset] = '\0';
    float value = atof(buff);
    *buffer += buffOffset + 1;//offset to next float
    return value;
}


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

    enum {BUFFSIZE = 256};
    char buff[BUFFSIZE];
int err;    
    while(fgets(buff, BUFFSIZE, mesh) != NULL) {

        if(isVertexCoordinates(buff)) {
            char* local = buff + 2;//skip whitespace and v tag

            float x = parceFloat(&local);
            float y = parceFloat(&local);
            float z = parceFloat(&local);

            vertexPosition.x = x;
            vertexPosition.y = y;
            vertexPosition.z = z;
 
            data.vertPos.push_back(vertexPosition);
            printf("v %f %f %f\n",x,y,z);
        
        }else if(isTextureCoordinates(buff)){
            char* local = buff + 3;//skip whitespace and vt tag

            float u = parceFloat(&local);
            float v = parceFloat(&local);
            float w = parceFloat(&local);

            textCoord.u = u;
            textCoord.v = v;
            textCoord.z = w;

            data.texCoord.push_back(textCoord);
            printf("vt %f %f %f\n", u, v, w);

        }else if(isVertexNormals(buff)){
            char* local = buff + 3;//skip whitespace and vn tag

            float x = parceFloat(&local);
            float y = parceFloat(&local);
            float z = parceFloat(&local);

            normals.x = x;
            normals.y = y;
            normals.z = z;

            data.normals.push_back(normals);
            printf("vn %f %f %f\n", x, y, z);
            
        }else if(isFaces(buff)) {
            Face face = {};
            char* local = buff + 2;//skip whitespace and f tag
            enum {
                VERT_ONLY = 0,// v
                VERT_TEXT, // v/vt
                VERT_NORM,// v//vn
                VERT_TEXT_NORM // v/vt/vn
            };

            //determine pattern
            uint8_t currIdx = 0;
            uint8_t firstSlashPos = -1;
            uint8_t secondSlashPos = -1;
            uint8_t thirdSlashPos = -1;

            uint8_t numSlashes = 0;
            bool twoSlashes = false;
            
            while(local[currIdx] && local[currIdx] != ' ') {
                if(local[currIdx] == '/') {
                    local[currIdx] = '\0';
                    int value = std::atoi(local);
                    printf("ASD\n");
                }
                currIdx++;
            }
            
        }
    }

    printf("MESH INFO:\n Faces = %d\n VertexPositions = %d\n Normals = %d\n Texture Coords = %d\n",
    data.faces.size(), data.vertPos.size(), data.normals.size(), data.texCoord.size());
    
    fclose(mesh);
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
