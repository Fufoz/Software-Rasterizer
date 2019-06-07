#ifndef MATH_H
#define MATH_H

#include <cstdint>
#include <stdio.h>
#include <cmath>

#define PI 3.14159265359

struct mat4x4
{
    float p[16];
};

union Vec2
{
    struct {
        float x;
        float y;
    };

    struct {
        float u;
        float v;
    };
    
    struct {
        float data[2];
    };

    inline float& operator[] (int idx) { return data[idx];}
    inline const float& operator[] (int idx) const { return data[idx];} 
};

union Vec3
{
    struct {
        float x;
        float y;
        float z;
    };
    
    struct {
        float u;
        float v;
        float _;
    };

    struct {
        float R;
        float G;
        float B;
    };

    struct {
        float data[3];
    };

    inline float& operator[] (int idx) { return data[idx];}
    inline const float& operator[] (int idx) const { return data[idx];} 
};

union Vec4
{
    struct {
        union {
            struct {
                float x;
                float y;
                float z;
            };
            Vec3 xyz;
        };

        float w;
    };

    struct {
        float R;
        float G;
        float B;
        float A;
    };

    struct {
        float data[4];
    };

    inline float& operator[] (int idx) { return data[idx];} 
    inline const float& operator[] (int idx) const { return data[idx];} 
};

union Quat
{
    struct {
        float x;
        float y;
        float z;
        float w;
    };

    struct {
        Vec3 complex;
        float scalar;
    };
    
    Vec4 xyzw;
};

/*********************VECTOR OPERATIONS************************************/

inline Vec2 operator+(const Vec2& left, const Vec2& right)
{
    return Vec2{left.x + right.x, left.y + right.y};
}

inline Vec2 operator-(const Vec2& left, const Vec2& right)
{
    return Vec2{left.x - right.x, left.y - right.y};
}

inline Vec2& operator+=(Vec2& self, const Vec2& other)
{
    self = self + other;
    return self;
}

inline Vec2& operator-=(Vec2& self, const Vec2& other)
{
    self = self - other;
    return self;
}

inline float dotVec2(const Vec2& left, const Vec2& right)
{
    return left.x * right.x + left.y * right.y; 
}

inline float lengthVec2(const Vec2& in)
{
    return sqrt(dotVec2(in, in));
}

inline Vec2 normaliseVec2(const Vec2& in)
{
    float length = lengthVec2(in);
    if(length > 0) {
        float invLength = 1 / length;
        return Vec2{in.x * invLength, in.y * invLength};
    }
    return Vec2{0.f, 0.f};
}

inline Vec3 operator+(const Vec3& left, const Vec3& right)
{
    return Vec3{left.x + right.x, left.y + right.y, left.z + right.z};
}

inline Vec3 operator-(const Vec3& left, const Vec3& right)
{
    return Vec3{left.x - right.x, left.y - right.y, left.z - right.z};
}

inline Vec3& operator+=(Vec3& self, const Vec3& other)
{
    self = self + other;
    return self;
}

inline Vec3& operator-=(Vec3& self, const Vec3& other)
{
    self = self - other;
    return self;
}

inline Vec3 operator*(float scalar,const Vec3& other)
{
    return Vec3{scalar * other.x, scalar * other.y, scalar * other.z};
}

inline Vec3 operator*(const Vec3& other, float scalar)
{
    return scalar * other;
}

inline float dotVec3(const Vec3& left, const Vec3& right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z; 
}

inline float lengthVec3(const Vec3& in)
{
    return sqrt(dotVec3(in, in));
}

inline Vec3 normaliseVec3(const Vec3& in)
{
    float length = lengthVec3(in);
    if(length > 0) {
        float invLength = 1 / length;
        return Vec3{in.x * invLength, in.y * invLength, in.z * invLength};
    }
    return Vec3{0.f, 0.f, 0.f};
}

inline Vec3 cross(const Vec3& first, const Vec3& second)
{
    return Vec3{
        first.y * second.z - first.z * second.y,
        first.z * second.x - first.x * second.z,
        first.x * second.y - first.y * second.x
    };
}

inline Vec4 homogenize(const Vec3& in)
{
    return Vec4{in.x, in.y, in.z, 1.f};
}

inline Vec4 perspectiveDivide(const Vec4& in)
{
    return Vec4 {in.x/in.w, in.y/in.w, in.z/in.w, 1.f};
}

inline float dotVec4(const Vec4& left, const Vec4& right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z + left.w * right.w; 
}

inline Vec4 operator*(float scalar,const Vec4& other)
{
    return Vec4{scalar * other.x, scalar * other.y, scalar * other.z, scalar * other.w};
}

inline Vec4 operator*(const Vec4& other, float scalar)
{
    return scalar * other;
}

inline Vec4 operator+(const Vec4& left, const Vec4& right)
{
    return Vec4{left.x + right.x, left.y + right.y, left.z + right.z, left.w + right.w};
}

inline Vec4 operator-(const Vec4& left, const Vec4& right)
{
    return Vec4{left.x - right.x, left.y - right.y, left.z - right.z, left.w - right.w};
}

inline Vec4& operator+=(Vec4& self, const Vec4& other)
{
    self = self + other;
    return self;
}

inline Vec4& operator-=(Vec4& self, const Vec4& other)
{
    self = self - other;
    return self;
}

/*********************4x4MAT OPERATIONS************************************/
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

    h = tan(FOV * 0.5f * PI / 180.f) * near;
    w = h * aspect;
    return frustum(-w, w, -h, h, near, far);
}

inline mat4x4 perspectiveGL(float FOV, float aspect, float near, float far)
{
    float angle = tan(FOV * 0.5f * PI / 180.f); 

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

    return translationPart * rotationPart;
}

inline mat4x4 rotateZ(float degrees)
{
    float rad = degrees * PI / 180.f;

    return mat4x4 {
        cosf(rad),  sinf(rad), 0, 0,
        -sinf(rad), cosf(rad), 0, 0,
        0,          0,         1, 0,
        0,          0,         0, 1
    };
}

inline mat4x4 rotateY(float degrees)
{
    float rad = degrees * PI / 180.f;
    return mat4x4 {
        cosf(rad), 0, -sinf(rad), 0,
        0,         1, 0,          0,
        sinf(rad), 0, cosf(rad),  0,
        0,         0, 0,          1
    };
}

inline mat4x4 rotateX(float degrees)
{
    float rad = degrees * PI / 180.f;
    return mat4x4 {
        1, 0,          0,          0,
        0, cosf(rad),  sinf(rad),  0,
        0, -sinf(rad), cosf(rad),  0,
        0, 0,          0,          1
    };
}

template<typename T>
inline T max(T a, T b)
{
    return a > b ? a : b;
}

template<typename T>
inline T min(T a, T b)
{
    return a > b ? b : a;
}

inline float lerp(float start, float end, float amount)
{
    return start + amount * (end - start);
}

inline Vec2 lerp(const Vec2& start, const Vec2& end, float amount)
{
    return Vec2 { 
        lerp(start.x, end.x, amount),
        lerp(start.y, end.y, amount)
    };
}

inline Vec3 lerp(const Vec3& start, const Vec3& end, float amount)
{
    return Vec3 { 
        lerp(start.x, end.x, amount),
        lerp(start.y, end.y, amount),
        lerp(start.z, end.z, amount)
    };
}

inline Vec4 lerp(const Vec4& start, const Vec4& end, float amount)
{
    return Vec4 {
        lerp(start.x, end.x, amount),
        lerp(start.y, end.y, amount),
        lerp(start.z, end.z, amount),
        lerp(start.w, end.w, amount)
    };
}

inline Quat conjugate(const Quat& in)
{
    return Quat{-in.x, -in.y, -in.z, in.w};
}

inline Quat operator*(const Quat& left, const Quat& right)
{
    Quat out = {};
    out.complex = left.complex * right.w + right.complex * left.w + cross(left.complex, right.complex);
    out.w = left.w * right.w - dotVec3(left.complex, right.complex);
    return out;
}

#endif