#ifndef VEC_H
#define VEC_H

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

#include <cmath>

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

#endif