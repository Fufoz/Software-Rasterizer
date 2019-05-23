#include "texture.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
bool loadTexture(const char* path, Texture* out, bool flipImage)
{
    int twidth;
    int theight;
    int numChannels;

    stbi_set_flip_vertically_on_load(flipImage);
    uint8_t* data = stbi_load(path, &twidth, &theight, &numChannels, STBI_rgb);
    
    if(!data) {
        printf("Failed to load texture from path %s!\n", path);
        return false;
    }

    out->width =  twidth;
    out->height = theight;
    out->numc = numChannels;
    out->data = data;
    return true; 
}