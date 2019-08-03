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
    while(**buffer == ' ')
        (*buffer)++;

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
            values[valueIdx] = atof(buff);
            if(buffer[currIdx + 1] == '/'){
                buff = &buffer[currIdx] + 2;//step past null tetminated char
                currIdx++;
            }
            else
                buff = &buffer[currIdx] + 1;
            valueIdx++;
        }

        if(buffer[currIdx + 1] == 0) {//eof line check
            values[valueIdx] = atof(buff);
        }

        currIdx++;
    }

    switch(pattern) {
        case PATTERN_VERT_ONLY : {
            face.vIndex[0] = values[0];
            face.vIndex[1] = values[1];
            face.vIndex[2] = values[2];
//            printf("Face %lu %lu %lu\n",face.vIndex[0], face.vIndex[1], face.vIndex[2]);
            break;
        }
        case PATTERN_VERT_NORM : {
            face.vIndex[0] = values[0];
            face.nIndex[0] = values[1];

            face.vIndex[1] = values[2];
            face.nIndex[1] = values[3];

            face.vIndex[2] = values[4];
            face.nIndex[2] = values[5];
//            printf("Face %lu//%lu %lu//%lu %lu//%lu\n",
//                face.vIndex[0],face.nIndex[0],
//                face.vIndex[1],face.nIndex[1],
//                face.vIndex[2],face.nIndex[2]);
            break;
        }
        case PATTERN_VERT_TEXT : {
            face.vIndex[0] = values[0];
            face.tIndex[0] = values[1];

            face.vIndex[1] = values[2];
            face.tIndex[1] = values[3];

            face.vIndex[2] = values[4];
            face.tIndex[2] = values[5];
//            printf("Face %lu/%lu %lu/%lu %lu/%lu\n",
//                face.vIndex[0],face.tIndex[0],
//                face.vIndex[1],face.tIndex[1],
//                face.vIndex[2],face.tIndex[2]);

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
//            printf("Face %lu/%lu/%lu %lu/%lu/%lu %lu/%lu/%lu\n",
//                face.vIndex[0], face.tIndex[0],face.nIndex[0],
//                face.vIndex[1], face.tIndex[1], face.nIndex[1],
//                face.vIndex[2], face.tIndex[2], face.nIndex[2]);

            break;
        }
        case PATTERN_UNKNOWN : {
            printf("Faces pattern is unknown!\n");
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

bool loadMesh(const char* model, Mesh* data)
{
    FILE* mesh = fopen(model, "rb");
    if(!mesh) {
        return false;
    }

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
 
            data->vertPos.push_back(vertexPosition);
            //printf("v %f %f %f\n",vertexPosition.x, vertexPosition.y, vertexPosition.z);
        
        }else if(isTextureCoordinates(buff)){
            char* local = buff + 3;//skip whitespace and vt tag

            textCoord.u = parceFloat(&local);
            textCoord.v = parceFloat(&local);
            textCoord.z = parceFloat(&local);

            data->texCoord.push_back(textCoord);
            //printf("vt %f %f %f\n", textCoord.u, textCoord.v, textCoord.z);

        }else if(isVertexNormals(buff)){
            char* local = buff + 3;//skip whitespace and vn tag

            normals.x = parceFloat(&local);
            normals.y = parceFloat(&local);
            normals.z = parceFloat(&local);

            data->normals.push_back(normals);
           // printf("vn %f %f %f\n", normals.x, normals.y, normals.z);
            
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
            data->faces.push_back(face);
        }
    }
    printf("----------------------------------------\n");
    printf("MESH INFO:\n Faces = %lu\n VertexPositions = %lu\n Normals = %lu\n Texture Coords = %lu\n",
        data->faces.size(), data->vertPos.size(), data->normals.size(), data->texCoord.size());
    printf("----------------------------------------\n");

    fclose(mesh);
    return true;
}

void averageNormals(Mesh* mesh)
{
    mesh->normals.resize(mesh->vertPos.size());
    memset(mesh->normals.data(), 0, mesh->normals.size() * sizeof(Vec3));

    for(uint32_t i = 0; i < mesh->faces.size(); i++) {
        //grab triangle vertices of current face
        Vec3& v0 = mesh->vertPos[mesh->faces[i].vIndex[0] - 1];
        Vec3& v1 = mesh->vertPos[mesh->faces[i].vIndex[1] - 1];
        Vec3& v2 = mesh->vertPos[mesh->faces[i].vIndex[2] - 1];
        
        //compute current face normal
        Vec3 firstEdge = v1 - v0;
        Vec3 secondEdge = v2 - v1;
        Vec3 normal = cross(firstEdge, secondEdge);
        normaliseVec3InPlace(normal);

        for(int j = 0; j < 3; j++) {
            mesh->normals[mesh->faces[i].vIndex[j] - 1] += normal;
            mesh->faces[i].nIndex[j] = mesh->faces[i].vIndex[j];
        }
    }

    for(uint32_t i = 0; i < mesh->normals.size(); i++) {
        normaliseVec3InPlace(mesh->normals[i]);
    }

    printf("Averaged normals:\n");
    for(auto normal : mesh->normals)
        printf("Normal %f %f %f\n", normal.x, normal.y, normal.z);

}

void fillTangent(Mesh* mesh)
{
    mesh->tangents.resize(mesh->vertPos.size());
    memset(mesh->tangents.data(), 0, mesh->tangents.size() * sizeof(Vec3));

    for(uint32_t i = 0; i < mesh->faces.size(); i++) {
        //grab triangle vertices of current face
        Vec3& v0 = mesh->vertPos[mesh->faces[i].vIndex[0] - 1];
        Vec3& v1 = mesh->vertPos[mesh->faces[i].vIndex[1] - 1];
        Vec3& v2 = mesh->vertPos[mesh->faces[i].vIndex[2] - 1];

        Vec3& t0 = mesh->texCoord[mesh->faces[i].tIndex[0] - 1];
        Vec3& t1 = mesh->texCoord[mesh->faces[i].tIndex[1] - 1];
        Vec3& t2 = mesh->texCoord[mesh->faces[i].tIndex[2] - 1];

        Vec3 firstEdge = v1 - v0;
        Vec3 secondEdge = v2 - v0;

        //compute tangent space basis vectors
        float deltaU1 = t1.u - t0.u;
        float deltaU2 = t2.u - t0.u;
        float deltaV1 = t1.v - t0.v;
        float deltaV2 = t2.v - t0.v;
        float invDet = 1.f / (deltaU1 * deltaV2 - deltaU2 * deltaV1);
        Vec3 T = normaliseVec3(deltaV2 * firstEdge - deltaV1 * secondEdge) * invDet;
        //normaliseVec3((deltaV2 - deltaU2) * firstEdge); 
        //Vec3 B = normaliseVec3((deltaU1 - deltaV1) * secondEdge);
        Vec3 B = normaliseVec3(-deltaU2 * firstEdge + deltaU1 * secondEdge) * invDet;
        for(int j = 0; j < 3; j++) {
            mesh->tangents[mesh->faces[i].vIndex[j] - 1] += T;
            mesh->faces[i].tanIndex[j] = mesh->faces[i].vIndex[j];
        }
    }

    for(uint32_t i = 0; i < mesh->faces.size(); i++) {
       
        /*
            Orthogonalize tangent vector for each vertex
            by using Gram-Schmit orthogonalisation
        */
        for(int j = 0; j < 3; j++) {
            const Vec3& normal = mesh->normals[mesh->faces[i].nIndex[j] - 1];
            Vec3& tangent = mesh->tangents[mesh->faces[i].tanIndex[j] - 1];
            tangent = normaliseVec3(tangent - dotVec3(tangent, normal) * normal);
        }
    }
    for(uint32_t i = 0 ; i < mesh->tangents.size(); i++) {
        printf("Tangent vector %f %f %f\n",mesh->tangents[i].x,mesh->tangents[i].y,mesh->tangents[i].z);
    }
}