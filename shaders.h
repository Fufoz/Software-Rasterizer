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
    virtual Vertex vertexShader(const Vertex& in, int vn) = 0;
    virtual Vec3 fragmentShader(const Vec3& pixelCoords, bool& discard) = 0;

    virtual void prepareInterpolants(
        const Vertex& v1, const Vertex& v2, const Vertex& v3, 
        float invZ1, float invZ2, float invZ3,
        float triArea) = 0;
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

    Vec3 fragmentShader(const Vec3& pixelCoords, bool& discard)
    {
        //normalise z values between 0 and 1
        float z = (pixelCoords.z - 0.1f) / (10.f - 0.1f);
        Vec3 gl_fragColor = Vec3{z * 255.f, z * 255.f, z * 255.f };
        return gl_fragColor;
    }

      void prepareInterpolants(
        const Vertex& v1, const Vertex& v2, const Vertex& v3, 
        float invZ1, float invZ2, float invZ3,
        float triArea)
        {
            
        }
};

struct FlatShader : Shader
{
    mat4x4 in_VP;
    Vec3 in_ambient;
    float intensity;

    Vertex vertexShader(const Vertex& in, int vn)
    {
        Vertex gl_Position = {};
        gl_Position.pos = in.pos * in_VP;
        return gl_Position;
    }

    Vec3 fragmentShader(const Vec3& pixelCoords, bool& discard)
    {
        Vec3 gl_fragColor = in_ambient * intensity;        
        return gl_fragColor;
    }

      void prepareInterpolants(
        const Vertex& v1, const Vertex& v2, const Vertex& v3, 
        float invZ1, float invZ2, float invZ3,
        float triArea)
        {
            
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

    Vertex vertexShader(const Vertex& in, int vn)
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

    Vec3 fragmentShader(const Vec3& pixelCoords, bool& discard)
    {        
        Vec3 gl_fragColor = interpContext.beginCoeffs.color + interpContext.w1 * interpContext.C1C0 + interpContext.w2 * interpContext.C2C0;
        return gl_fragColor;
    }
      void prepareInterpolants(
        const Vertex& v1, const Vertex& v2, const Vertex& v3, 
        float invZ1, float invZ2, float invZ3,
        float triArea)
        {

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

    Vertex vertexShader(const Vertex& in, int vn)
    {
        Vertex gl_Position = {};
        gl_Position.pos = in.pos * in_VP;
        gl_Position.normal = normaliseVec3(in.normal * in_normalTransform);
        return gl_Position;
    }

    Vec3 fragmentShader(const Vec3& pixelCoords, bool& discard)
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
    
  void prepareInterpolants(
        const Vertex& v1, const Vertex& v2, const Vertex& v3, 
        float invZ1, float invZ2, float invZ3,
        float triArea){}
};

//bump mapping(a.k.a normal mapping)
struct BumpShader : Shader {

    mat4x4 in_VP;
    mat4x4 in_normalTransform;
    Vec3 in_cameraPosition;

    Vec3 lightVector[3];
    Vec3 viewVector[3];
    Vec3 uvs[3];

    Texture* sampler2d;
    Texture* sampler2dN;
    Texture* sampler2dD;
    Vec3 ambientReflectivity = {0.1f, 0.1f, 0.1f};
    Vec3 diffuseReflectivity = {1.f, 1.f, 1.f};
    Vec3 specularReflectivity = {1.f, 1.f, 1.f};
    int glossinessPower = 4;

    Vec3 V1V0,V2V0,L1L0,L2L0,T1T0,T2T0;
    
    Vertex vertexShader(const Vertex& in, int vn)
    {
        Vertex gl_Position = {};

        //we're currently assuming that light comes from the same
        // spot where the camera is
        Vec3 view = normaliseVec3(in_cameraPosition - in.pos.xyz);
        Vec3 light = view;

        gl_Position.normal = normaliseVec3(in.normal * in_normalTransform);
        gl_Position.tangent = normaliseVec3(in.tangent * in_normalTransform);
        Vec3 bitangent = normaliseVec3(cross(gl_Position.normal, gl_Position.tangent));

        //move view and light vectors to tangent space
        viewVector[vn].x = dotVec3(view, gl_Position.tangent);
        viewVector[vn].y = dotVec3(view, bitangent);
        viewVector[vn].z = dotVec3(view, gl_Position.normal);

        lightVector[vn].x = dotVec3(light, gl_Position.tangent);
        lightVector[vn].y = dotVec3(light, bitangent);
        lightVector[vn].z = dotVec3(light, gl_Position.normal);

        gl_Position.texCoords = in.texCoords;

        gl_Position.pos = in.pos * in_VP;

        return gl_Position;
    }

    void prepareInterpolants(
        const Vertex& v1, const Vertex& v2, const Vertex& v3, 
        float invZ1, float invZ2, float invZ3,
        float triArea)
    {
        
        lightVector[0] *= invZ1;
        lightVector[1] *= invZ2;
        lightVector[2] *= invZ3;

        L1L0 = (lightVector[1] - lightVector[0]) / triArea;
        L2L0 = (lightVector[2] - lightVector[0]) / triArea;

        viewVector[0] *= invZ1;
        viewVector[1] *= invZ2;
        viewVector[2] *= invZ3;

        V1V0 = (viewVector[1] - viewVector[0]) / triArea;
        V2V0 = (viewVector[2] - viewVector[0]) / triArea;
        
        uvs[0] = v1.texCoords * invZ1;
        uvs[1] = v2.texCoords * invZ2;
        uvs[2] = v3.texCoords * invZ3;

        T1T0 = (uvs[1] - uvs[0]) / triArea;
        T2T0 = (uvs[2] - uvs[0]) / triArea;
    }

    Vec3 fragmentShader(const Vec3& pixelCoords, bool& discard)
    {
        //sample texture
        Vec3 interpUVs = (uvs[0] + pixelCoords.u * T1T0 + pixelCoords.v * T2T0) * pixelCoords.z; 
        Vec3 interpLight = (lightVector[0] + pixelCoords.u * L1L0 + pixelCoords.v * L2L0) * pixelCoords.z; 
        Vec3 interpView = (viewVector[0] + pixelCoords.u * V1V0 + pixelCoords.v * V2V0) * pixelCoords.z; 
        interpLight = normaliseVec3(interpLight);
        interpView = normaliseVec3(interpView);

        int numLayers = 30;
        float layerStep = 1.f / (float)numLayers;
        float currentDiscreteHeight = 0.f;
        Vec2 uvStep = interpView.xy* 0.2f/(float)numLayers;///(float)(interpView.z * numLayers);
        float currentSampledDepth = sampleTexture1ch(sampler2dD, interpUVs.xy)/255.f;

        for(uint32_t i = 0; i < numLayers; i++) {
            interpUVs.u += uvStep.u;
            interpUVs.v -= uvStep.v;
            currentDiscreteHeight += layerStep;
            currentSampledDepth = sampleTexture1ch(sampler2dD, interpUVs.xy)/255.f;
            
            if(currentDiscreteHeight > currentSampledDepth)
                break;
        }

       // Vec3 prevUVs = {};
       // prevUVs.u = interpUVs.u - uvStep.u;
       // prevUVs.v = interpUVs.v + uvStep.v;
//
       // float nextDepth = currentSampledDepth - currentDiscreteHeight;
       // float previousSampledDepth = texture2D(sampler2dD, prevUVs) - currentDiscreteHeight + layerStep;
       // float weight = previousSampledDepth / (previousSampledDepth - currentSampledDepth);
       // float actualDepth = lerp(currentSampledDepth, previousSampledDepth, weight);
	    
        //discard fragments at texture border
        if(interpUVs.u > 1 || interpUVs.u < 0 || interpUVs.v > 1 || interpUVs.v < 0) {
            discard = true;
            return Vec3{};
        }

		Vec3 color = sampleTexture3ch(sampler2d, interpUVs.xy);

        //decode normal coords from normal map        
        Vec3 normal = (sampleTexture3ch(sampler2dN, interpUVs.xy) - 128.f)/128.f; 
        normal = normaliseVec3(normal);

        Vec3 gl_fragColor = {};
        Vec3 diffuseContribution = diffuseReflectivity * max(0.f, dotVec3(interpLight, normal));
        Vec3 reflectedVector = 2.f * dotVec3(normal, interpLight) * normal - interpLight;
        Vec3 specularContribution = specularReflectivity * pow(max(0.f, dotVec3(reflectedVector, interpView)), glossinessPower);
        Vec3 ambientContribution = ambientReflectivity;
        gl_fragColor = (diffuseContribution + specularContribution + ambientContribution) ^ color;
        gl_fragColor = clamp(gl_fragColor, RGB_BLACK, RGB_WHITE);
        return gl_fragColor;
    }
    
};

#endif