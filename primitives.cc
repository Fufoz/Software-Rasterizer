#include "primitives.h"

#include <cassert>
#include "clipper.h"


void drawPixel(const SDL_Surface* surface, int x, int y, Vec3 color)
{
    assert(x < surface->w && y < surface->h);
    assert(x >= 0 && y >= 0);

	uint32_t* pixelPtr = (uint32_t*)surface->pixels;

	pixelPtr += surface->w * (surface->h - 1); 
    
    pixelPtr += x - surface->w * y;
	*pixelPtr = SDL_MapRGBA(surface->format, color.R, color.G, color.B, 255.f);
}

void drawLine(const SDL_Surface* surface, int x0, int y0, int x1, int y1, Vec3 color)
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

void drawWireFrame(const SDL_Surface* surface, Vec4 v0, Vec4 v1, Vec4 v2, Vec3 color)
{
    v0 = perspectiveDivide(v0) * viewportTransform;
    v1 = perspectiveDivide(v1) * viewportTransform;
    v2 = perspectiveDivide(v2) * viewportTransform;

    drawLine(surface, v0.x, v0.y, v1.x, v1.y, color);
    drawLine(surface, v1.x, v1.y, v2.x, v2.y, color);
    drawLine(surface, v2.x, v2.y, v0.x, v0.y, color);
}

void drawTriangleHalfSpaceFlat(RenderContext* context, Vertex v0, Vertex v1, Vertex v2, Shader& shader)
{
    float* zBuffer = context->rtargets.zBuffer;
    SDL_Surface* surface = context->surface;
       
    //preserve depth of a polygon via keeping its z coordinate in clip-space
    float z0Inv = 1.f / (float)v0.pos.w;
    float z1Inv = 1.f / (float)v1.pos.w;
    float z2Inv = 1.f / (float)v2.pos.w;
    
    v0.pos = perspectiveDivide(v0.pos) * viewportTransform;
    v1.pos = perspectiveDivide(v1.pos) * viewportTransform;
    v2.pos = perspectiveDivide(v2.pos) * viewportTransform;

    const float triArea = computeArea(v0.pos.xyz, v1.pos.xyz, v2.pos.xyz);
    if(triArea < 0)
        return;
    
    shader.prepareInterpolants(v0, v1, v2, z0Inv, z1Inv, z2Inv, triArea);
    //compute triangle bounding box
    int topY   = max(max(v0.pos.y, v1.pos.y), v2.pos.y);
    int leftX  = min(min(v0.pos.x, v1.pos.x), v2.pos.x);
    int botY   = min(min(v0.pos.y, v1.pos.y), v2.pos.y);
    int rightX = max(max(v0.pos.x, v1.pos.x), v2.pos.x);
    
    //clamp against pixel space coords
    //topY = clamp(topY, 0, context->window.height - 1);
    //leftX = clamp(leftX, 0, context->window.width - 1);
    //botY = clamp(botY, 0, context->window.height - 1);
    //rightX = clamp(rightX, 0, context->window.width - 1);
    
    //calculate row and column step in barycentric coordinates
    float A01 = v0.pos.y - v1.pos.y;
    float B01 = v1.pos.x - v0.pos.x;

    float A12 = v1.pos.y - v2.pos.y;
    float B12 = v2.pos.x - v1.pos.x;

    float A20 = v2.pos.y - v0.pos.y;
    float B20 = v0.pos.x - v2.pos.x;

    float Z1Z0Inv = (z1Inv - z0Inv) / triArea;
    float Z2Z0Inv = (z2Inv - z0Inv) / triArea;

    float w0StartRow = computeArea(v1.pos.xyz, v2.pos.xyz, Vec3{(float)leftX, (float)topY, 0});
    float w1StartRow = computeArea(v2.pos.xyz, v0.pos.xyz, Vec3{(float)leftX, (float)topY, 0});
    float w2StartRow = computeArea(v0.pos.xyz, v1.pos.xyz, Vec3{(float)leftX, (float)topY, 0});
    bool discardFragment = false;

    for(int y = topY; y > botY; y--) {

        float w0 = w0StartRow;
        float w1 = w1StartRow;
        float w2 = w2StartRow;

        for(int x = leftX; x <= rightX; x++) {
			//proper fill rule handling is too time consuming in tight rasterizer loops
            if( w0 >=0 && w1>=0 && w2>=0) {              
                //we're basically interpolating depth values in camera space
                //to get perspective correct interpolation
                float Z = z0Inv + w1 * Z1Z0Inv + w2 * Z2Z0Inv;
                //to get back to screen space
                Z = 1.f / Z;
                if( Z < zBuffer[y * surface->w + x]) {
                    zBuffer[y * surface->w + x] = Z;
                    Vec3 gl_pixelCoord = {w1, w2, Z};
                    discardFragment = false;
                    Vec3 finalColor = shader.fragmentShader(gl_pixelCoord, discardFragment);
                    if(!discardFragment)
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


static const uint8_t precision = 4;
//triangle setup for each sample
//interate in subpixel steps
void rasterizeTriangle4xMSAA(RenderContext* context, Vertex v0, Vertex v1, Vertex v2, Shader& shader)
{
    float* zBuffer = context->rtargets.zBuffer;
    uint8_t* cBuffer = context->rtargets.cBuffer;    
    SDL_Surface* surface = context->surface;
       
    //preserve depth of a polygon via keeping its z coordinate in clip-space
    float z0Inv = 1.f / (float)v0.pos.w;
    float z1Inv = 1.f / (float)v1.pos.w;
    float z2Inv = 1.f / (float)v2.pos.w;
    
    v0.pos = perspectiveDivide(v0.pos) * viewportTransform;
    v1.pos = perspectiveDivide(v1.pos) * viewportTransform;
    v2.pos = perspectiveDivide(v2.pos) * viewportTransform;
    
    const float triArea = computeArea(v0.pos.xyz, v1.pos.xyz, v2.pos.xyz);
    if(triArea < 0)
        return;
    
    shader.prepareInterpolants(v0, v1, v2, z0Inv, z1Inv, z2Inv, triArea);
    
    //compute triangle bounding box
    int topY   = max(max(v0.pos.y, v1.pos.y), v2.pos.y);
    int leftX  = min(min(v0.pos.x, v1.pos.x), v2.pos.x);
    int botY   = min(min(v0.pos.y, v1.pos.y), v2.pos.y);
    int rightX = max(max(v0.pos.x, v1.pos.x), v2.pos.x);
    //clamp against pixel space coords
    topY = clamp(topY, 0, context->window.height - 1);
    leftX = clamp(leftX, 0, context->window.width - 1);
    botY = clamp(botY, 0, context->window.height - 1);
    rightX = clamp(rightX, 0, context->window.width - 1);
    //calculate row and column step in barycentric coordinates
    float A01 = v0.pos.y - v1.pos.y;
    float B01 = v1.pos.x - v0.pos.x;

    float A12 = v1.pos.y - v2.pos.y;
    float B12 = v2.pos.x - v1.pos.x;

    float A20 = v2.pos.y - v0.pos.y;
    float B20 = v0.pos.x - v2.pos.x;

    float Z1Z0Inv = (z1Inv - z0Inv) / triArea;
    float Z2Z0Inv = (z2Inv - z0Inv) / triArea;

    float w0StartRow = computeArea(v1.pos.xyz, v2.pos.xyz, Vec3{(float)leftX, (float)topY, 0});
    float w1StartRow = computeArea(v2.pos.xyz, v0.pos.xyz, Vec3{(float)leftX, (float)topY, 0});
    float w2StartRow = computeArea(v0.pos.xyz, v1.pos.xyz, Vec3{(float)leftX, (float)topY, 0});
    bool discardFragment = false;
    
    //msaa setup
    int step = 0x10;
    int subMask = 0xf;
    v0.pos *= step;
    v1.pos *= step;
    v2.pos *= step;

    //compute triangle bounding box
    int subTopY   = max(max(v0.pos.y, v1.pos.y), v2.pos.y);
    int subLeftX  = min(min(v0.pos.x, v1.pos.x), v2.pos.x);
    int subBotY   = min(min(v0.pos.y, v1.pos.y), v2.pos.y);
    int subRightX = max(max(v0.pos.x, v1.pos.x), v2.pos.x);

    subTopY = (subTopY + subMask) & (~subMask);
    subLeftX = (subLeftX + subMask) & (~subMask);
    subBotY = (subBotY + subMask) & (~subMask);
    subRightX = (subRightX + subMask) & (~subMask);

    for(int y = subTopY; y > subBotY; y-=step) {

        float w0 = w0StartRow;
        float w1 = w1StartRow;
        float w2 = w2StartRow;
//printf("loop %d\n",y);
        for(int x = subLeftX; x <= subRightX; x+=step) {
			//proper fill rule handling is too time consuming in tight rasterizer loops
            if( w0 >=0 && w1>=0 && w2>=0) {              
                //we're basically interpolating depth values in camera space
                //to get perspective correct interpolation
                float Z = z0Inv + w1 * Z1Z0Inv + w2 * Z2Z0Inv;
                //to get back to screen space
                Z = 1.f / Z;
                if( Z < zBuffer[y>>4 * surface->w + x>>4]) {
                    zBuffer[y>>4 * surface->w + x>>4] = Z;
                    Vec3 gl_pixelCoord = {w1, w2, Z};
                    discardFragment = false;
                    Vec3 finalColor = shader.fragmentShader(gl_pixelCoord, discardFragment);
                    if(!discardFragment)
                        drawPixel(surface, x>>4, y>>4, finalColor);
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
