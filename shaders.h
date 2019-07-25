#ifndef SHADER_H
#define SHADER_H
#include "renderer.h"

enum InterpFeatureBits
{
    FEATURE_HAS_COLOR_BIT = 1 << 0,
    FEATURE_HAS_TEXTURE_BIT = 1 << 1,
    FEATURE_HAS_NORMAL_BIT = 1 << 2
};
typedef uint32_t InterpFeaturesFlagBits;

struct BeginCoeffs
{
    Vec3 color;
    Vec3 normal;
    Vec3 uv;
};

struct InterpContext
{
    InterpFeaturesFlagBits interpFeatures;
    BeginCoeffs beginCoeffs;
    float w1;//barycentric coords
    float w2;//barycentric coords
    float T2T0x;
    float T1T0x;
    float T2T0y;
    float T1T0y;
    float W1W0;
    float W2W0;
    Vec3 C1C0;
    Vec3 C2C0;
    Vec3 N1N0;
    Vec3 N2N0;

};

struct Shader {
    InterpContext interpContext;
    virtual Vertex vertexShader(const Vertex& in) = 0;
    virtual Vec3 fragmentShader(const Vec3& pixelCoords) = 0;
};

struct DepthShader : Shader
{
    mat4x4 in_VP;

    Vertex vertexShader(const Vertex& in)
    {
        Vertex gl_Position = {};
        gl_Position.pos = in.pos * in_VP;
        return gl_Position;
    }

    Vec3 fragmentShader(const Vec3& pixelCoords)
    {
        //normalise z values between 0 and 1
        float z = (pixelCoords.z - 0.1f) / (10.f - 0.1f);
        Vec3 gl_fragColor = Vec3{z * 255.f, z * 255.f, z * 255.f };
        return gl_fragColor;
    }
};

struct FlatShader : Shader
{
    mat4x4 in_VP;
    Vec3 in_ambient;
    float intensity;

    Vertex vertexShader(const Vertex& in)
    {
        Vertex gl_Position = {};
        gl_Position.pos = in.pos * in_VP;
        return gl_Position;
    }

    Vec3 fragmentShader(const Vec3& pixelCoords)
    {
        Vec3 gl_fragColor = in_ambient * intensity;        
        return gl_fragColor;
    }
};

struct GouraudShader : Shader
{
    mat4x4 in_VP;
    Vec3 in_lightVector;
    Vec3 in_viewVector;
    Vec3 in_Color;

    Vec3 ambientReflectivity = {0.1f, 0.1f, 0.1f};
    Vec3 diffuseReflectivity = {1.f, 1.f, 1.f};
    Vec3 specularReflectivity = {1.f, 1.f, 1.f};
    int glossinessPower = 32;

    Vertex vertexShader(const Vertex& in)
    {
        Vertex gl_Position = {};
        gl_Position.pos = in.pos * in_VP;

        Vec3 reflectedVector = 2.f * dotVec3(in.normal, in_lightVector) * in.normal - in_lightVector;
        
        //here we're assuming that light intencity is {1,1,1}
        Vec3 lightIntencity = ambientReflectivity
            + diffuseReflectivity * max(0.f, dotVec3(in_lightVector, in.normal))
            + specularReflectivity * pow(max(0.f, dotVec3(reflectedVector, in_viewVector)), glossinessPower);

        gl_Position.color = clamp(lightIntencity ^ in_Color, RGB_BLACK, RGB_WHITE);
        return gl_Position;
    }

    Vec3 fragmentShader(const Vec3& pixelCoords)
    {        
        Vec3 gl_fragColor = interpContext.beginCoeffs.color + interpContext.w1 * interpContext.C1C0 + interpContext.w2 * interpContext.C2C0;
        return gl_fragColor;
    }
};

struct PhongShader : Shader
{
    mat4x4 in_VP;
    mat4x4 in_normalTransform;
    Vec3 in_lightVector;
    Vec3 in_viewVector;
    Vec3 in_Color;

    Vec3 ambientReflectivity = {0.1f, 0.1f, 0.1f};
    Vec3 diffuseReflectivity = {1.f, 1.f, 1.f};
    Vec3 specularReflectivity = {1.f, 1.f, 1.f};
    int glossinessPower = 32;

    Vertex vertexShader(const Vertex& in)
    {
        Vertex gl_Position = {};
        gl_Position.pos = in.pos * in_VP;
        gl_Position.normal = normaliseVec3(in.normal * in_normalTransform);
        return gl_Position;
    }

    Vec3 fragmentShader(const Vec3& pixelCoords)
    {
        Vec3 normal = interpContext.beginCoeffs.normal + interpContext.w1 * interpContext.N1N0 + interpContext.w2 * interpContext.N2N0;
        normal = normal * pixelCoords.z;
        normal = normaliseVec3(normal);

        Vec3 gl_fragColor = {};
        Vec3 diffuseContribution = diffuseReflectivity * max(0.f, dotVec3(in_lightVector, normal));
        Vec3 reflectedVector = 2.f * dotVec3(normal, in_lightVector) * normal - in_lightVector;
        Vec3 specularContribution = specularReflectivity * pow(max(0.f, dotVec3(reflectedVector, in_viewVector)), glossinessPower);
        Vec3 ambientContribution = ambientReflectivity;
        gl_fragColor = (diffuseContribution + specularContribution + ambientContribution) ^ in_Color;
        gl_fragColor = clamp(gl_fragColor, RGB_BLACK, RGB_WHITE);
        return gl_fragColor;
    }
};

//bump mapping(a.k.a normal mapping)
struct BumpShader : Shader {
    mat4x4 in_VP;
    mat4x4 in_normalTransform;
    Vec3 in_lightVector;
    Vec3 in_viewVector;
    Vec3 in_Color;
    Texture *sampler2d;

    Vec3 ambientReflectivity = {0.1f, 0.1f, 0.1f};
    Vec3 diffuseReflectivity = {1.f, 1.f, 1.f};
    Vec3 specularReflectivity = {1.f, 1.f, 1.f};
    int glossinessPower = 32;

    Vertex vertexShader(const Vertex& in)
    {
        Vertex gl_Position = {};
        gl_Position.pos = in.pos * in_VP;
        gl_Position.normal = normaliseVec3(in.normal * in_normalTransform);
        return gl_Position;
    }

    Vec3 fragmentShader(const Vec3& pixelCoords)
    {
        float Tx = interpContext.beginCoeffs.uv.x +  interpContext.w1 * interpContext.T1T0x + interpContext.w2 * interpContext.T2T0x;
        float Ty = interpContext.beginCoeffs.uv.y +  interpContext.w1 * interpContext.T1T0y + interpContext.w2 * interpContext.T2T0y;
        Tx *= pixelCoords.z;
        Ty *= pixelCoords.z;

        int tx = Tx * (sampler2d->width - 1);
        int ty = Ty * (sampler2d->height - 1);
        int textureOffset = tx * 3 + ty * 3 * sampler2d->width;
        uint8_t* position = sampler2d->data + textureOffset;
        Vec3 color = {position[0], position[1], position[2]};

        Vec3 normal = interpContext.beginCoeffs.normal + interpContext.w1 * interpContext.N1N0 + interpContext.w2 * interpContext.N2N0;
        normal = normal / pixelCoords.z; 
        normal = normaliseVec3(normal);
//
        Vec3 gl_fragColor = {};
        Vec3 diffuseContribution = diffuseReflectivity * max(0.f, dotVec3(in_lightVector, normal));
        Vec3 reflectedVector = 2.f * dotVec3(normal, in_lightVector) * normal - in_lightVector;
        Vec3 specularContribution = specularReflectivity * pow(max(0.f, dotVec3(reflectedVector, in_viewVector)), glossinessPower);
        Vec3 ambientContribution = ambientReflectivity;
        gl_fragColor = (diffuseContribution + specularContribution + ambientContribution) ^ color;
        gl_fragColor = clamp(gl_fragColor, RGB_BLACK, RGB_WHITE);
        return gl_fragColor;
    }
};

#endif