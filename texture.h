#ifndef TEXTURE_H
#define TEXTURE_H
#include <cstdint>

struct Texture
{
    uint32_t width;
    uint32_t height;
    uint8_t numc;
    uint8_t* data;
};

bool loadTexture(const char* path, Texture* out, bool flipImage = false);

#endif