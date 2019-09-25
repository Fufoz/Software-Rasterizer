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

void drawTriangleHalfSpace(RenderContext* context, Vertex v0, Vertex v1, Vertex v2, Shader& shader)
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
	//28.4 fixed format
	int x0 = std::floor(16.f * v0.pos.x + 0.5f);
	int x1 = std::floor(16.f * v1.pos.x + 0.5f);
	int x2 = std::floor(16.f * v2.pos.x + 0.5f);
	int y0 = std::floor(16.f * v0.pos.y + 0.5f);
	int y1 = std::floor(16.f * v1.pos.y + 0.5f);
	int y2 = std::floor(16.f * v2.pos.y + 0.5f);

    //compute triangle bounding box
    int topY   = min((max(max(y0, y1), y2)) >> 4, context->window.height - 1);
    int leftX  = max((min(min(x0, x1), x2)) >> 4, 0);
    int botY   = max((min(min(y0, y1), y2)) >> 4, 0);
    int rightX = min((max(max(x0, x1), x2)) >> 4, context->window.width - 1);

    //calculate row and column step in barycentric coordinates
    int A01 = y0 - y1;
    int B01 = x1 - x0;

    int A12 = y1 - y2;
    int B12 = x2 - x1;

    int A20 = y2 - y0;
    int B20 = x0 - x2;

    int FA01 = int(unsigned(A01) << 4);
    int FB01 = int(unsigned(B01) << 4);
    int FA12 = int(unsigned(A12) << 4);
    int FB12 = int(unsigned(B12) << 4);
    int FA20 = int(unsigned(A20) << 4);
    int FB20 = int(unsigned(B20) << 4);
	
    float Z1Z0Inv = (z1Inv - z0Inv) / triArea;
    float Z2Z0Inv = (z2Inv - z0Inv) / triArea;
	
	int fleftX = leftX << 4;
	int ftopY =  topY << 4;

    int w0StartRow = (x2 - x1) * (ftopY - y1) - (fleftX - x1) * (y2 - y1);
	int w1StartRow = (x0 - x2) * (ftopY - y2) - (fleftX - x2) * (y0 - y2);
	int w2StartRow = (x1 - x0) * (ftopY - y0) - (fleftX - x0) * (y1 - y0);

    bool discardFragment = false;

    for(int y = topY; y > botY; y--) {

        int w0 = w0StartRow;
        int w1 = w1StartRow;
        int w2 = w2StartRow;

        for(int x = leftX; x <= rightX; x++) {
			//proper fill rule handling is too time consuming in tight rasterizer loops
            if( w0>=0 && w1 >=0 && w2>=0) {              
                //we're basically interpolating depth values in camera space
                //to get perspective correct interpolation
                float Z = z0Inv + (w1 >> 8) * Z1Z0Inv + (w2 >> 8) * Z2Z0Inv;
                //to get back to screen space
                Z = 1.f / Z;
                if( Z < zBuffer[y * surface->w + x]) {
                    zBuffer[y * surface->w + x] = Z;
                    Vec3 gl_pixelCoord = {w1>>8, w2>>8, Z};
                    discardFragment = false;
                    Vec3 finalColor = shader.fragmentShader(gl_pixelCoord, discardFragment);
                    if(!discardFragment)
                        drawPixel(surface, x, y, finalColor);
                }
            }
			
            w0 += FA12;
            w1 += FA20;
            w2 += FA01;
        }

        w0StartRow -= FB12;
        w1StartRow -= FB20;
        w2StartRow -= FB01;
    }
}

struct SampleRastInfo
{
	float w0StartRow;
	float w1StartRow;
	float w2StartRow;
	int FA01;
	int FB01;
	int FA12;
	int FB12;
	int FA20;
	int FB20;
};

SampleRastInfo prepareSample(RenderContext* context, Vec3 v0, Vec3 v1, Vec3 v2, uint8_t sX, uint8_t sY)
{
	SampleRastInfo info = {};
	//28.4 fixed format
	int x0 = std::floor(16.f * v0.x + 0.5f) + sX;
	int x1 = std::floor(16.f * v1.x + 0.5f) + sX;
	int x2 = std::floor(16.f * v2.x + 0.5f) + sX;
	int y0 = std::floor(16.f * v0.y + 0.5f) + sY;
	int y1 = std::floor(16.f * v1.y + 0.5f) + sY;
	int y2 = std::floor(16.f * v2.y + 0.5f) + sY;

    //compute triangle bounding box
    int topY   = min((max(max(y0, y1), y2)), context->window.height - 1);
    int leftX  = max((min(min(x0, x1), x2)), 0);
    int botY   = max((min(min(y0, y1), y2)), 0);
    int rightX = min((max(max(x0, x1), x2)), context->window.width - 1);
	
	topY /= 16.f;
	leftX /= 16.f;
	botY /= 16.f;
	rightX /= 16.f;

    //calculate row and column step in barycentric coordinates
    int A01 = y0 - y1;
    int B01 = x1 - x0;

    int A12 = y1 - y2;
    int B12 = x2 - x1;

    int A20 = y2 - y0;
    int B20 = x0 - x2;

    int FA01 = int(unsigned(A01) << 4);
    int FB01 = int(unsigned(B01) << 4);
    int FA12 = int(unsigned(A12) << 4);
    int FB12 = int(unsigned(B12) << 4);
    int FA20 = int(unsigned(A20) << 4);
    int FB20 = int(unsigned(B20) << 4);

	int fleftX = leftX << 4;
	int ftopY =  topY << 4;

    int w0StartRow = (x2 - x1) * (ftopY - y1) - (fleftX - x1) * (y2 - y1);
	int w1StartRow = (x0 - x2) * (ftopY - y2) - (fleftX - x2) * (y0 - y2);
	int w2StartRow = (x1 - x0) * (ftopY - y0) - (fleftX - x0) * (y1 - y0);
	
	info.w0StartRow = w0StartRow;
	info.w1StartRow = w1StartRow;
	info.w2StartRow = w2StartRow;
	info.FA01 = FA01;
	info.FB01 = FB01;
	info.FA12 = FA12;
	info.FB12 = FB12;
	info.FA20 = FA20;
	info.FB20 = FB20;

	return info;
}

void drawTriangleHalfSpaceMSAA(RenderContext* context, Vertex v0, Vertex v1, Vertex v2, Shader& shader)
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
	//28.4 fixed format
	int x0 = std::floor(16.f * v0.pos.x + 0.5f);
	int x1 = std::floor(16.f * v1.pos.x + 0.5f);
	int x2 = std::floor(16.f * v2.pos.x + 0.5f);
	int y0 = std::floor(16.f * v0.pos.y + 0.5f);
	int y1 = std::floor(16.f * v1.pos.y + 0.5f);
	int y2 = std::floor(16.f * v2.pos.y + 0.5f);

    //compute triangle bounding box
    int topY   = min((max(max(y0, y1), y2)) >> 4, context->window.height - 1);
    int leftX  = max((min(min(x0, x1), x2)) >> 4, 0);
    int botY   = max((min(min(y0, y1), y2)) >> 4, 0);
    int rightX = min((max(max(x0, x1), x2)) >> 4, context->window.width - 1);

    //calculate row and column step in barycentric coordinates
    int A01 = y0 - y1;
    int B01 = x1 - x0;

    int A12 = y1 - y2;
    int B12 = x2 - x1;

    int A20 = y2 - y0;
    int B20 = x0 - x2;

    int FA01 = int(unsigned(A01) << 4);
    int FB01 = int(unsigned(B01) << 4);
    int FA12 = int(unsigned(A12) << 4);
    int FB12 = int(unsigned(B12) << 4);
    int FA20 = int(unsigned(A20) << 4);
    int FB20 = int(unsigned(B20) << 4);
	
    float Z1Z0Inv = (z1Inv - z0Inv) / triArea;
    float Z2Z0Inv = (z2Inv - z0Inv) / triArea;
	
	int fleftX = leftX << 4;
	int ftopY =  topY << 4;

    int w0StartRow = (x2 - x1) * (ftopY - y1) - (fleftX - x1) * (y2 - y1);
	int w1StartRow = (x0 - x2) * (ftopY - y2) - (fleftX - x2) * (y0 - y2);
	int w2StartRow = (x1 - x0) * (ftopY - y0) - (fleftX - x0) * (y1 - y0);

    bool discardFragment = false;

	SampleRastInfo s1 = prepareSample(context, v0.pos.xyz, v1.pos.xyz, v2.pos.xyz, sampleLocX[0], sampleLocY[0]);
	SampleRastInfo s2 = prepareSample(context, v0.pos.xyz, v1.pos.xyz, v2.pos.xyz, sampleLocX[1], sampleLocY[1]);
	SampleRastInfo s3 = prepareSample(context, v0.pos.xyz, v1.pos.xyz, v2.pos.xyz, sampleLocX[2], sampleLocY[2]);
	SampleRastInfo s4 = prepareSample(context, v0.pos.xyz, v1.pos.xyz, v2.pos.xyz, sampleLocX[3], sampleLocY[3]);

    for(int y = topY; y > botY; y--) {

        int w0 = w0StartRow;
        int w1 = w1StartRow;
        int w2 = w2StartRow;

        int w0s1 = s1.w0StartRow;
        int w1s1 = s1.w1StartRow;
        int w2s1 = s1.w2StartRow;

        int w0s2 = s2.w0StartRow;
        int w1s2 = s2.w1StartRow;
        int w2s2 = s2.w2StartRow;

        int w0s3 = s3.w0StartRow;
        int w1s3 = s3.w1StartRow;
        int w2s3 = s3.w2StartRow;

        int w0s4 = s4.w0StartRow;
        int w1s4 = s4.w1StartRow;
        int w2s4 = s4.w2StartRow;

        for(int x = leftX; x <= rightX; x++) {
			//proper fill rule handling is too time consuming in tight rasterizer loops
            if( w0>=0 && w1 >=0 && w2>=0) {              
                //we're basically interpolating depth values in camera space
                //to get perspective correct interpolation
                float Z = z0Inv + (w1 >> 8) * Z1Z0Inv + (w2 >> 8) * Z2Z0Inv;
                //to get back to screen space
                Z = 1.f / Z;
                if( Z < zBuffer[y * surface->w + x]) {
                    zBuffer[y * surface->w + x] = Z;
                    Vec3 gl_pixelCoord = {w1>>8, w2>>8, Z};
                    discardFragment = false;
                    Vec3 finalColor = shader.fragmentShader(gl_pixelCoord, discardFragment);
                    if(!discardFragment)
                        drawPixel(surface, x, y, finalColor);
                }
            }
			
            w0 += FA12;
            w1 += FA20;
            w2 += FA01;

            w0s1 += s1.FA12;
            w1s1 += s1.FA20;
            w2s1 += s1.FA01;

            w0s2 += s2.FA12;
            w1s2 += s2.FA20;
            w2s2 += s2.FA01;

            w0s3 += s3.FA12;
            w1s3 += s3.FA20;
            w2s3 += s3.FA01;

            w0s4 += s4.FA12;
            w1s4 += s4.FA20;
            w2s4 += s4.FA01;
        }

        w0StartRow -= FB12;
        w1StartRow -= FB20;
        w2StartRow -= FB01;

        s1.w0StartRow -= s1.FB12;
        s1.w1StartRow -= s1.FB20;
        s1.w2StartRow -= s1.FB01;

        s2.w0StartRow -= s2.FB12;
        s2.w1StartRow -= s2.FB20;
        s2.w2StartRow -= s2.FB01;

        s3.w0StartRow -= s3.FB12;
        s3.w1StartRow -= s3.FB20;
        s3.w2StartRow -= s3.FB01;

        s4.w0StartRow -= s4.FB12;
        s4.w1StartRow -= s4.FB20;
        s4.w2StartRow -= s4.FB01;

    }
}