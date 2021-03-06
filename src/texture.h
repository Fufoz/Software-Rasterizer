#ifndef TEXTURE_H
#define TEXTURE_H

#include "maths.h"
#include <cstdint>
#include <stddef.h>

struct Texture
{
	size_t size;
	uint32_t width;
	uint32_t height;
	uint8_t numc;
	uint8_t* data;
};

bool loadTexture(const char* path, Texture* out, bool flipImage = false);
void unloadTexture(void* handle);
Vec3 sampleTexture3ch(Texture* sampler, Vec2 uvs);
uint8_t sampleTexture1ch(Texture* sampler, Vec2 uvs);

#endif