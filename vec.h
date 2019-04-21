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

float lengthVec2(const Vec2& in);
float dotVec2(const Vec2& left, const Vec2& right);
Vec2 normaliseVec2(const Vec2& in);
Vec2 addVec2(const Vec2& left, const Vec2& right);
Vec2 subVec2(const Vec2& left, const Vec2& right);

float lengthVec3(const Vec3& in);
float dotVec3(const Vec3& left, const Vec3& right);
Vec3 normaliseVec3(const Vec3& in);
Vec3 cross(const Vec3& first, const Vec3& second);
Vec3 addVec3(const Vec3& left, const Vec3& right);
Vec3 subVec3(const Vec3& left, const Vec3& right);

float multiply(const Vec3& left, const Vec3& right);

#endif