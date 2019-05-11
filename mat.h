#ifndef MAT_H
#define MAT_H

#include <cstdint>
#define _USE_MATH_DEFINES
#include <math.h>
#include <stdio.h>
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
inline mat4x4 viewport(float screenWidth, float screenHeight)
{
    return mat4x4 {
        screenWidth/2.f,        0,                   0, 0,
        0,                    screenHeight/2.f,      0, 0,
        0,                    0,                     1, 0,
        screenWidth/2.f-0.5f, screenHeight/2.f-0.5f, 0, 1
    };

}

inline mat4x4 frustum(float left, float right, float bottom, float top, float near, float far)
{
    return mat4x4 {
        (2.f*near)/(right - left),   0,                          0,                          0,
        0,                         2.f*near/(top-bottom),        (top+bottom)/(top-bottom),  0,
        (right+left)/(right-left), (top+bottom)/(top-bottom),  -(far + near)/(far-near),    -1.f,
        0,                         0,                          (-2.f*far*near)/(far-near),      0
    };
}

inline mat4x4 perspectiveProjection(float FOV, float aspect, float near, float far)
{
    float h;
    float w;

    h = tan(FOV * 0.5f * M_PI / 180.f) * near;
    w = h * aspect;
    return frustum(-w, w, -h, h, near, far);
}

inline mat4x4 perspectiveGL(float FOV, float aspect, float near, float far)
{
    float angle = tan(FOV * 0.5f * M_PI / 180.f); 

    return mat4x4 {
        angle/aspect, 0,     0,                              0,
        0,            angle, 0,                              0,
        0,            0,      -(far+near)/(far-near),        -1,
        0,            0,      -2 * far * near / (far - near), 0
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
    out.x = left.x * right.p[0] + left.y * right.p[4] + left.z * right.p[8] +  left.w * right.p[12];
    out.y = left.x * right.p[1] + left.y * right.p[5] + left.z * right.p[9] +  left.w * right.p[13];
    out.z = left.x * right.p[2] + left.y * right.p[6] + left.z * right.p[10] + left.w * right.p[14];
    out.w = left.x * right.p[3] + left.y * right.p[7] + left.z * right.p[11] + left.w * right.p[15];
    return out;
}

inline Vec4& operator*=(Vec4& left, const mat4x4& right)
{
    left = left * right;
    return left;
}

inline Vec3 operator*(const Vec3& left, const mat4x4& right)
{
    Vec4 out = {left.x, left.y, left.z, 1.f};
    out = out * right;
    return out.xyz;
}


inline void logMat4x4(const char* tag, const mat4x4& in)
{
    printf("-------------------%s--------------------\n",tag);
    for(uint8_t i = 0; i < 4; i++) {
        for(uint8_t j = 0; j < 4; j++) {
            printf("%f ",in.p[j + 4 * i]);
        }
        printf("\n");
    }
    printf("---------------------------------------\n");
}

inline mat4x4 lookAt(Vec3 cameraPos, Vec3 thing, Vec3 UpDir = Vec3{0.f, 1.f, 0.f})
{
    Vec3 Z  = normaliseVec3(cameraPos - thing);
    Vec3 At = cross(normaliseVec3(UpDir), Z);
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
}

inline mat4x4 rotateZ(float degrees)
{
    float rad = degrees * M_PI / 180.f;

    return mat4x4 {
        cos(rad), sin(rad), 0, 0,
        -sin(rad), cos(rad), 0, 0,
        0,         0,        1, 0,
        0,         0,        0, 1
    };
}

inline mat4x4 rotateY(float degrees)
{
    float rad = degrees * M_PI / 180.f;
    return mat4x4 {
        cos(rad), 0, -sin(rad), 0,
        0,        1, 0,         0,
        sin(rad), 0, cos(rad),  0,
        0,        0, 0,         1
    };
}

inline mat4x4 rotateX(float degrees)
{
    float rad = degrees * M_PI / 180.f;
    return mat4x4 {
        1, 0,         0,         0,
        0, cos(rad),  sin(rad),  0,
        0, -sin(rad), cos(rad),  0,
        0, 0,         0,         1
    };
}

#endif