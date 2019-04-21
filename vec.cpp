#include "vec.h"
#include <cmath>


float dotVec2(const Vec2& left, const Vec2& right)
{
    return left.x * right.x + left.y * right.y; 
}

float lengthVec2(const Vec2& in)
{
    return sqrt(dotVec2(in, in));
}

Vec2 normaliseVec2(const Vec2& in)
{
    float length = lengthVec2(in);
    if(length > 0) {
        float invLength = 1 / length;
        return Vec2{in.x * invLength, in.y * invLength};
    }
    return Vec2{0.f, 0.f};
}

Vec2 addVec2(const Vec2& left, const Vec2& right)
{
    return Vec2{left.x + right.x, left.y + right.y};
}

Vec2 subVec2(const Vec2& left, const Vec2& right)
{
    return Vec2{left.x - right.x, left.y - right.y};
}


float dotVec3(const Vec3& left, const Vec3& right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z; 
}

float lengthVec3(const Vec3& in)
{
    return sqrt(dotVec3(in, in));
}

Vec3 normaliseVec3(const Vec3& in)
{
    float length = lengthVec3(in);
    if(length > 0) {
        float invLength = 1 / length;
        return Vec3{in.x * invLength, in.y * invLength, in.z * invLength};
    }
    return Vec3{0.f, 0.f, 0.f};
}

Vec3 addVec3(const Vec3& left, const Vec3& right)
{
    return Vec3{left.x + right.x, left.y + right.y, left.z + right.z};
}

Vec3 subVec3(const Vec3& left, const Vec3& right)
{
    return Vec3{left.x - right.x, left.y - right.y, left.z - right.z};
}

Vec3 cross(const Vec3& first, const Vec3& second)
{
    return Vec3{
        first.y * second.z - first.z * second.y,
        first.z * second.x - first.x * second.z,
        first.x * second.y - first.y * second.x
    };
}

float multiply(const Vec3& left, const Vec3& right)
{
    return left.x * right.x + left.y * right.y + left.z * right.z;
}
