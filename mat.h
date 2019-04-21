#ifndef MAT_H
#define MAT_H

#include <cstdint>
#include <cmath>
#include "vec.h"

struct mat4x4
{
    float p[16];
};

inline mat4x4 loadIdentity()
{
    return mat4x4 {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };
}

inline mat4x4 loadTranslation(const Vec3& vec)
{
    return mat4x4 {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        vec.x, vec.y, vec.z, 1
    };
}

inline mat4x4 loadScale(const Vec3& vec)
{
    return mat4x4 {
        vec.x, 0, 0, 0,
        0, vec.y, 0, 0,
        0, 0, vec.z, 0,
        0, 0, 0, 1
    };
}

inline mat4x4 simplePerspective(Vec3 v)
{
    return mat4x4 {
        1.f/(1.f-v.z/4.f), 0, 0, 0,
        0, 1.f/(1.f-v.z/4.f), 0, 0,
        0, 0, 1, 0,
        0, 0, 0, 1
    };

}

inline mat4x4 setProjectionMatrix(float angleOfView, float near, float far, float aspectRatio) 
{ 
    //set the basic projection matrix
    float scale = 1 / tan(angleOfView * 0.5f * M_PI / 180.f);
    return mat4x4 {
        scale / aspectRatio, 0    , 0,                0,
        0    , scale, 0,                              0,
        0,    0,     - (far + near) / (far - near),  -1,
        0,    0,     - 2 * far * near / (far - near), 0
    };

}


inline mat4x4 transpose(const mat4x4& in)
{
    mat4x4 out = {};

    for(uint8_t i = 0; i < 4; i++) {
        for(uint8_t j = 0; j < 4; j++) {
            out.p[j + i * 4] = in.p[j * 4 + i];
        }
    }
    return out;
}

inline void transposeInplace(mat4x4& in)
{
    mat4x4 tmp = in;

    for(uint8_t i = 0; i < 4; i++) {
        for(uint8_t j = 0; j < 4; j++) {
            in.p[j + i * 4] = tmp.p[j * 4 + i];
        }
    }
}

inline mat4x4 operator*(const mat4x4& left, const mat4x4& right)
{
    mat4x4 result = {};
    const uint8_t stride = 4;

    for(uint8_t i = 0; i < 4; i++) {
        for(uint8_t j = 0; j < 4; j++) {
            for(uint8_t k = 0; k < 4; k++) {
                result.p[j + stride * i] += left.p[k + stride * i] * right.p[k * stride + j];
            }
        }
    }
    return result;
}

inline Vec4 operator*(const Vec4& left, const mat4x4& right)
{
    Vec4 out = {};
    out.x = left.x * right.p[0] + left.y * right.p[4] + left.z * right.p[8] + right.p[12];
    out.y = left.x * right.p[1] + left.y * right.p[5] + left.z * right.p[9] + right.p[13];
    out.z = left.x * right.p[2] + left.y * right.p[6] + left.z * right.p[10] + right.p[14];
    float w = left.x * right.p[3] + left.y * right.p[7] + left.z * right.p[11] + right.p[15];
    if(w != 1) {
        out.x /= w;
        out.y /= w;
        out.z /= w;
    }
    return out;
}


inline Vec3 operator*(const Vec3& left, const mat4x4& right)
{
    Vec4 out = {left.x, left.y, left.z, 1.f};
    out = out * right;
    return out.xyz;
}

inline void logMat4x4(const mat4x4& in)
{
    for(uint8_t i = 0; i < 4; i++) {
        for(uint8_t j = 0; j < 4; j++) {
            printf("%f.4 ",in.p[j + 4 * i]);
        }
        printf("\n");
    }
}

mat4x4 lookAt(Vec3 cameraPos, Vec3 thing, Vec3 UpDir)
{
    Vec3 Z  = normaliseVec3(subVec3(cameraPos, thing));
    Vec3 At = cross(normaliseVec3(UpDir),Z);
    Vec3 Up = cross(Z, At);

    mat4x4 rotationPart = {
        At.x, Up.x, Z.x, 0,
        At.y, Up.y, Z.y, 0,
        At.z, Up.z, Z.z, 0,
        0,    0,    0,   1
    };
    mat4x4 translationPart  = loadIdentity();
    translationPart.p[12] = -thing.x;
    translationPart.p[13] = -thing.y;
    translationPart.p[14] = -thing.z;

    return rotationPart * translationPart;
//    return mat4x4 {
//        At.x, Up.x, Z.x, 0,
//        At.y, Up.y, Z.y, 0,
//        At.z, Up.z, Z.z, 0,
//        -thing.x, -thing.y, -thing.z, 1
//    };

}

#endif