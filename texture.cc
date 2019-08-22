#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

bool loadTexture(const char* path, Texture* out, bool flipImage)
{
    int twidth;
    int theight;
    int numChannels;

    stbi_set_flip_vertically_on_load(flipImage);
    uint8_t* data = stbi_load(path, &twidth, &theight, &numChannels, 0);
    
    if(!data) {
        printf("Failed to load texture from path %s!\n", path);
        return false;
    }

    out->width =  twidth;
    out->height = theight;
    out->numc = numChannels;
    out->data = data;
    out->size = twidth * theight * numChannels;
    return true; 
}

Vec3 sampleTexture3ch(Texture* sampler, Vec2 uvs)
{
    if(uvs.u > 1 || uvs.u < 0 || uvs.v > 1 || uvs.v < 0)
        return Vec3{};
    int xOffset = std::floor(uvs.u * (sampler->width - 1)) * sampler->numc; 
    int yOffset = std::floor(uvs.v * (sampler->height - 1)) * sampler->numc;
    uint8_t* pos = sampler->data + xOffset + yOffset * sampler->width;
    return Vec3{pos[0], pos[1], pos[2]};
}

uint8_t sampleTexture1ch(Texture* sampler, Vec2 uvs)
{
    if(uvs.u > 1 || uvs.u < 0 || uvs.v > 1 || uvs.v < 0)
        return 0;
    int xOffset = std::floor(uvs.u * (sampler->width - 1)) * sampler->numc; 
    int yOffset = std::floor(uvs.v * (sampler->height - 1)) * sampler->numc;
    uint8_t* pos = sampler->data + xOffset + yOffset * sampler->width;
    return pos[0];
}

void unloadTexture(void* handle)
{
    stbi_image_free(handle);
}