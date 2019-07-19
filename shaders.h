#ifndef SHADER_H
#define SHADER_H
#include "renderer.h"

#define MAX_INTERP_FEATURES 3

enum InterpFeatureBits
{
    FEATURE_HAS_COLOR_BIT = 1 << 0,
    FEATURE_HAS_TEXTURE_BIT = 1 << 1,
};

struct BeginCoeffs
{
    Vec3 color;
    Vec2 uv;
};

struct InterpContext
{
    InterpFeatureBits interpFeatures;
    BeginCoeffs beginCoeffs;
    float w1;//barycentric coords
    float w2;//barycentric coords
    float T2T0;
    float T1T0;
    Vec3 C1C0;
    Vec3 C2C0;
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
    Vec3 in_ambient;
    Vec3 in_lightVector;
    size_t vId = 0;

    Vertex vertexShader(const Vertex& in)
    {
        Vertex gl_Position = {};
        gl_Position.pos = in.pos * in_VP;
        float lightIntencity = max(0.f, dotVec3(in_lightVector, in.normal));

        gl_Position.color = in_ambient * lightIntencity;
        
        return gl_Position;
    }

    Vec3 fragmentShader(const Vec3& pixelCoords)
    {        
        Vec3 gl_fragColor = interpContext.beginCoeffs.color + interpContext.w1 * interpContext.C1C0 + interpContext.w2 * interpContext.C2C0;
        return gl_fragColor;
    }
};

#endif