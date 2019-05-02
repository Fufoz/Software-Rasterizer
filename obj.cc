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

static Face parseFace(FacePattern pattern, char* buffer)
{
    Face face = {};
    char* buff = buffer;

    int values[9] = {};
    uint8_t currIdx = 0;
    uint8_t valueIdx = 0;

    while(buffer[currIdx]) {

        if(buffer[currIdx] == '/' || buffer[currIdx] == ' ') {
            buffer[currIdx] = '\0';
            values[valueIdx] = std::atof(buff);
            if(buffer[currIdx + 1] == '/'){
                buff = &buffer[currIdx] + 2;//step past null tetminated char
                currIdx++;
            }
            else
                buff = &buffer[currIdx] + 1;
            valueIdx++;
        }

        if(buffer[currIdx + 1] == 0) {//eof line check
            values[valueIdx] = std::atof(buff);
        }

        currIdx++;
    }

    switch(pattern) {
        case PATTERN_VERT_ONLY : {
            face.vIndex[0] = values[0];
            face.vIndex[1] = values[1];
            face.vIndex[2] = values[2];
            printf("Face %d %d %d\n",face.vIndex[0], face.vIndex[1], face.vIndex[2]);
            break;
        }
        case PATTERN_VERT_NORM : {
            face.vIndex[0] = values[0];
            face.nIndex[0] = values[1];

            face.vIndex[1] = values[2];
            face.nIndex[1] = values[3];

            face.vIndex[2] = values[4];
            face.nIndex[2] = values[5];
            printf("Face %d//%d %d//%d %d//%d\n",
                face.vIndex[0],face.nIndex[0],
                face.vIndex[1],face.nIndex[1],
                face.vIndex[2],face.nIndex[2]);
            break;
        }
        case PATTERN_VERT_TEXT : {
            face.vIndex[0] = values[0];
            face.tIndex[0] = values[1];

            face.vIndex[1] = values[2];
            face.tIndex[1] = values[3];

            face.vIndex[2] = values[4];
            face.tIndex[2] = values[5];
            printf("Face %d/%d %d/%d %d/%d\n",
                face.vIndex[0],face.tIndex[0],
                face.vIndex[1],face.tIndex[1],
                face.vIndex[2],face.tIndex[2]);

            break;
        }
        case PATTERN_VERT_TEXT_NORM : {
            face.vIndex[0] = values[0];
            face.tIndex[0] = values[1];
            face.nIndex[0] = values[2];

            face.vIndex[1] = values[3];
            face.tIndex[1] = values[4];
            face.nIndex[1] = values[5];

            face.vIndex[2] = values[6];
            face.tIndex[2] = values[7];
            face.nIndex[2] = values[8];
            printf("Face %d/%d/%d %d/%d/%d %d/%d/%d\n",
                face.vIndex[0], face.tIndex[0],face.nIndex[0],
                face.vIndex[1], face.tIndex[1], face.nIndex[1],
                face.vIndex[2], face.tIndex[2], face.nIndex[2]);

            break;
        }

    }


    return face;
}

const char* patToStr(FacePattern pattern) {
    switch(pattern) {
        case PATTERN_VERT_ONLY : return "PATTERN_VERT_ONLY";
        case PATTERN_VERT_TEXT : return "PATTERN_VERT_TEXT";
        case PATTERN_VERT_NORM : return "PATTERN_VERT_NORM";
        case PATTERN_VERT_TEXT_NORM : return "PATTERN_VERT_TEXT_NORM";
        default : return "PATTERN_VERT_ONLY";
    }
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
                return PATTERN_VERT_NORM;
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

    FacePattern ptrn = PATTERN_UNKNOWN;
    Vec3 vertexPosition = {};
    Vec3 textCoord = {};
    Vec3 normals = {};

    enum {BUFFSIZE = 256};
    char buff[BUFFSIZE] = {};
    
    printf("MESH:\n");
    printf("----------------------------------------\n");
    while(fgets(buff, BUFFSIZE, mesh) != NULL) {

        if(isVertexCoordinates(buff)) {
            char* local = buff + 2;//skip whitespace and v tag

            vertexPosition.x = parceFloat(&local);
            vertexPosition.y = parceFloat(&local);
            vertexPosition.z = parceFloat(&local);
 
            data.vertPos.push_back(vertexPosition);
            printf("v %f %f %f\n",vertexPosition.x, vertexPosition.y, vertexPosition.z);
        
        }else if(isTextureCoordinates(buff)){
            char* local = buff + 3;//skip whitespace and vt tag

            textCoord.u = parceFloat(&local);
            textCoord.v = parceFloat(&local);
            textCoord.z = parceFloat(&local);

            data.texCoord.push_back(textCoord);
            printf("vt %f %f %f\n", normals.u, normals.v, normals.z);

        }else if(isVertexNormals(buff)){
            char* local = buff + 3;//skip whitespace and vn tag

            normals.x = parceFloat(&local);
            normals.y = parceFloat(&local);
            normals.z = parceFloat(&local);

            data.normals.push_back(normals);
            printf("vn %f %f %f\n", normals.x, normals.y, normals.z);
            
        }else if(isFaces(buff)) {
            Face face = {};
            char* local = buff + 2;//skip whitespace and f tag
            if(ptrn == PATTERN_UNKNOWN){
                ptrn = checkFacePattern(local);
                printf("--Pattern: %s\n",patToStr(ptrn));
            } 
            if(ptrn == PATTERN_UNKNOWN) {
                printf("Error parsing obj file! Unknown face pattern!\n");
                return false;
            }
            face = parseFace(ptrn, local);
            data.faces.push_back(face);
        }
    }
    printf("----------------------------------------\n");
    printf("MESH INFO:\n Faces = %d\n VertexPositions = %d\n Normals = %d\n Texture Coords = %d\n",
        data.faces.size(), data.vertPos.size(), data.normals.size(), data.texCoord.size());
    printf("----------------------------------------\n");
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
