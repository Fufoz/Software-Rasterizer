#include "primitives.h"

#include <cassert>
#include <algorithm>

namespace primitives {

void drawPixel(const SDL_Surface* surface, int x, int y, Vec4 color)
{
    //assert(x < surface->w && y < surface->h);
    //assert(x >= 0 && y >= 0);
//
	//uint32_t* pixelPtr = (uint32_t*)surface->pixels;
//
    //pixelPtr += y * surface->w + x;    
	//*pixelPtr = SDL_MapRGBA(surface->format, color.R, color.G, color.B, color.A);

    assert(x < surface->w && y < surface->h);
    assert(x >= 0 && y >= 0);

	uint32_t* pixelPtr = (uint32_t*)surface->pixels;

	pixelPtr += surface->w * (surface->h - 1); 
    
    pixelPtr += x - surface->w * y;
	*pixelPtr = SDL_MapRGBA(surface->format, color.R, color.G, color.B, color.A);
}

void drawLine(const SDL_Surface* surface, int x0, int y0, int x1, int y1, Vec4 color)
{
    bool steep = false;
    if(std::abs(x1 - x0) < std::abs(y1 - y0)) {
        std::swap(x0, y0);
        std::swap(x1, y1);
        steep = true;
    }

    if(x0 > x1) {//if we are drawing backwards just swap coordinates order
        std::swap(x0, x1);
        std::swap(y0, y1);
    }

    float slope = std::abs((float)(y1 - y0) / (x1 - x0));
    float error = 0;
//    float offset = y0 - slope*x0;

    int y = y0;

    for(int x = x0; x < x1; x++) {
        if(steep)
            drawPixel(surface, y, x, color);
        else
            drawPixel(surface, x, y, color);

        error += slope;
        if(error > 0.5f) {
            y += (y1 > y0 ? 1 : -1);
            error -= 1;
        }
    }
}

static float computeArea(Vec3 v0, Vec3 v1, Vec3 v2)
{
    //return (v2.x - v0.x) * (v1.y - v0.y) - (v1.x - v0.x) * (v2.y - v0.y);
    return (v1.x - v0.x) * (v2.y - v0.y) - (v2.x  - v0.x) * (v1.y - v0.y); 
}

void drawWireFrame(const SDL_Surface* surface, const mat4x4& viewportTransform, Vec4 v0, Vec4 v1, Vec4 v2, Vec4 color)
{
    v0 = perspectiveDivide(v0) * viewportTransform;
    v1 = perspectiveDivide(v1) * viewportTransform;
    v2 = perspectiveDivide(v2) * viewportTransform;

    drawLine(surface, v0.x, v0.y, v1.x, v1.y, color);
    drawLine(surface, v1.x, v1.y, v2.x, v2.y, color);
    drawLine(surface, v2.x, v2.y, v0.x, v0.y, color);
}

static bool isInsideClipBox(const Vec4& v0)
{
return v0.x <= v0.w && v0.y <=v0.w && v0.z <= v0.w &&
        v0.x >= -v0.w && v0.y >= -v0.w && v0.z >= -v0.w; 
}

void drawTriangleHalfSpace(const SDL_Surface* surface, const mat4x4& viewportTransform,
    std::vector<float>& zBuffer,const Texture& texture, Vertex v0, Vertex v1, Vertex v2, Vec4 color)
{
    
    v0.pos = perspectiveDivide(v0.pos)  * viewportTransform;
    v1.pos = perspectiveDivide(v1.pos)  * viewportTransform;
    v2.pos = perspectiveDivide(v2.pos)  * viewportTransform;

    float triArea = computeArea(v0.pos.xyz, v1.pos.xyz, v2.pos.xyz)/2.f;
    
    //backface culling
    if(triArea < 0)
        return;
    //compute triangle bounding box
    int topY   = std::max(std::max(v0.pos.y, v1.pos.y), v2.pos.y);
    int leftX  = std::min(std::min(v0.pos.x, v1.pos.x), v2.pos.x);
    int botY   = std::min(std::min(v0.pos.y, v1.pos.y), v2.pos.y);
    int rightX = std::max(std::max(v0.pos.x, v1.pos.x), v2.pos.x);

    //clip bbox:
    leftX = std::max(leftX, 0);
    botY = std::max(botY, 0);
    topY = std::min(topY, surface->h - 1);
    rightX = std::min(rightX, surface->w - 1);

    float A01 = v0.pos.y - v1.pos.y;
    float B01 = v1.pos.x - v0.pos.x;

    float A12 = v1.pos.y - v2.pos.y;
    float B12 = v2.pos.x - v1.pos.x;

    float A20 = v2.pos.y - v0.pos.y;
    float B20 = v0.pos.x - v2.pos.x;

    float Z1Z0 = (v1.pos.z - v0.pos.z) / triArea;
    float Z2Z0 = (v2.pos.z - v0.pos.z) / triArea; 

    float T1T0x = (v1.texCoords.x - v0.texCoords.x) / triArea;
    float T1T0y = (v1.texCoords.y - v0.texCoords.y) / triArea;

    float T2T0x = (v2.texCoords.x - v0.texCoords.x) / triArea;
    float T2T0y = (v2.texCoords.y - v0.texCoords.y) / triArea;

    float w0StartRow = computeArea(v1.pos.xyz, v2.pos.xyz, Vec3{(float)leftX, (float)topY, 0});
    float w1StartRow = computeArea(v2.pos.xyz, v0.pos.xyz, Vec3{(float)leftX, (float)topY, 0});
    float w2StartRow = computeArea(v0.pos.xyz, v1.pos.xyz, Vec3{(float)leftX, (float)topY, 0});


    for(int y = topY; y > botY; y--) {

        float w0 = w0StartRow;
        float w1 = w1StartRow;
        float w2 = w2StartRow;

        for(int x = leftX; x <= rightX; x++) {
        
            if( ((int)w0|(int)w1|(int)w2)>=0) {
                float Z = v0.pos.z + w1 * Z1Z0 + w2 * Z2Z0;

                if(zBuffer[y * surface->w + x] < Z) {
                    
                    zBuffer[y * surface->w + x] = Z;
                    float Tx = v0.texCoords.x + w1 * T1T0x + w2 * T2T0x;
                    float Ty = v0.texCoords.y + w1 * T1T0y + w2 * T2T0y;
                    int texutreOffset = texture.width * std::floor(Ty * texture.height) * 3 + std::floor(Tx * texture.width) * 3;
                    uint8_t* position = texture.data + texutreOffset;
                    Vec4 finalColor;
                    //finalColor.R = position[0] * color.R;
                    //finalColor.G = position[1] * color.G;
                    //finalColor.B = position[2] * color.B;
                    finalColor.R = color.R;// * 255;
                    finalColor.G = color.G;// * 255;
                    finalColor.B = color.B;// * 255;
                    finalColor.A = 255;

                    drawPixel(surface, x, y, finalColor);
                }
            }

            w0 += A12;
            w1 += A20;
            w2 += A01;

        }

        w0StartRow -= B12;
        w1StartRow -= B20;
        w2StartRow -= B01;
    }

}

}//primitives