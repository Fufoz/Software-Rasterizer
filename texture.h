#ifndef TEXTURE_H
#define TEXTURE_H
#include <cstdint>

struct Texture
{
    size_t width;
    size_t height;
    uint8_t numc;
    uint8_t* data;
};

bool loadTexture(const char* path, Texture* out, bool flipImage = false);

#endif