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

enum FacePattern {
    PATTERN_VERT_ONLY = 0,// v
    PATTERN_VERT_TEXT, // v/vt
    PATTERN_VERT_NORM,// v//vn
    PATTERN_VERT_TEXT_NORM, // v/vt/vn
    PATTERN_UNKNOWN
};

static Face parseFace(FacePattern pattern, char** buffer)
{
    Face face = {};
    char* buff = *buffer;

    int values[3] = {};
    uint8_t currIdx = 0;
    uint8_t valueIdx = 0;
    
    while(buff[currIdx] && buff[currIdx] != ' ')
    {
        if(buff[currIdx] == '/') {
            buff[currIdx] = '\0';
            values[valueIdx] = atof(buff);
            buff += currIdx;
            valueIdx++;
        }
        currIdx++;
    }

//    switch(pattern) {
//        case PATTERN_VERT_ONLY :
//            face
//    }
}

static FacePattern checkFacePattern(const char* faceString)
{
    FacePattern pattern = PATTERN_UNKNOWN;

    //determine pattern
    uint8_t currIdx = 0;
    uint8_t numSlashes = 0;

    while(faceString[currIdx] != ' ') {
        if(faceString[currIdx] == '/') {
            numSlashes++;
            if(faceString[currIdx + 1] == '/') {
                numSlashes++;
                pattern = PATTERN_VERT_NORM;
                break;
            }
        }
        currIdx++;
    }

    if(numSlashes == 0)
        pattern = PATTERN_VERT_ONLY;
    else if(numSlashes == 1)
        pattern = PATTERN_VERT_TEXT;
    else if(numSlashes == 2)
        pattern = PATTERN_VERT_TEXT_NORM;
    else
        pattern = PATTERN_UNKNOWN;
    
    printf("pattern is %d\n",pattern);
    return pattern;
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
            FacePattern ptrn = checkFacePattern(local); 
            if(ptrn == PATTERN_UNKNOWN) {
                printf("Error parsing obj file! Unknown face pattern!\n");
                return false;
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
