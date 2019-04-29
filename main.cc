
#include <stdio.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <algorithm>
#include "obj.h"
#include "timer.h"
#include <limits>
#include <xmmintrin.h>
#include "mat.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

struct Texture
{
    size_t width;
    size_t height;
    uint8_t numc;
    uint8_t* data;
};

struct Color{
	float r;
	float g;
	float b;
	float a;
};

bool Running = false;
std::vector<float> zBuffer;
Texture texture = {};

static void drawPixel(SDL_Surface* surface, int x, int y, Color color)
{
    assert(x < surface->w && y < surface->h);
    assert(x >= 0 && y >= 0);

	uint32_t* pixelPtr = (uint32_t*)surface->pixels;

	pixelPtr += surface->w * (surface->h - 1); 
    
    pixelPtr += x - surface->w * y;
	*pixelPtr = SDL_MapRGBA(surface->format, color.r, color.g, color.b, color.a);
}



static void drawLine(SDL_Surface* surface, int x0, int y0, int x1, int y1, Color color)
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
    return (v1.x - v0.x) * (v2.y - v0.y) - (v2.x  - v0.x) * (v1.y - v0.y); 
}

static void drawWireFrame(SDL_Surface* surface, Vec3 v0, Vec3 v1, Vec3 v2, Color color)
{
    float area = computeArea(v0, v1, v2);
    if(area < 0)
        return;
    drawLine(surface, v0.x, v0.y, v1.x, v1.y, color);
    drawLine(surface, v1.x, v1.y, v2.x, v2.y, color);
    drawLine(surface, v2.x, v2.y, v0.x, v0.y, color);
}

static void drawTriangleHalfSpace(SDL_Surface* surface, Vertex v0, Vertex v1, Vertex v2, Color color)
{
    float area = computeArea(v0.pos.xyz, v1.pos.xyz, v2.pos.xyz);

    if(area < 0)
        return;

    float triArea = computeArea(v0.pos.xyz, v1.pos.xyz, v2.pos.xyz);

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
                    Color tcol = {};
                    tcol.r = position[0] * color.r;
                    tcol.g = position[1] * color.g;
                    tcol.b = position[2] * color.b;
                   // tcol.r = color.r * 255;
                   //tcol.g = color.g * 255;
                   //tcol.b = color.b * 255;

                    tcol.a = 255;
                    drawPixel(surface, x, y, tcol);
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



int main(int argc, char **argv)
{

  	SDL_Init(SDL_INIT_VIDEO);
    int width = 640;
    int height = 480;

  	SDL_Window *window = SDL_CreateWindow(
  	  "SDL2Test",
  	  SDL_WINDOWPOS_UNDEFINED,
  	  SDL_WINDOWPOS_UNDEFINED,
  	  width,
  	  height,
  	  SDL_WINDOW_RESIZABLE
  	);

    int twidth;
    int theight;
    int numChannels;
    stbi_set_flip_vertically_on_load(1);
    uint8_t* data = stbi_load("./african_head_diffuse.tga",&twidth, &theight, &numChannels, STBI_rgb);
    uint8_t* temp = data;
    printf("R=%d G=%d B=%d A=%d\n",temp[0], temp[1], temp[2], temp[3]);
    assert(data);
    texture.width =  twidth;
    texture.height = theight;
    texture.numc = numChannels;
    texture.data = data;

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_SOFTWARE);
	SDL_Surface *surface = SDL_GetWindowSurface(window);
	SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 211,211,211,255));

	Running = true;
	SDL_Event event;
	uint32_t FPSCounter = 0;
    Timer fpsTimer;

    ObjModel model;
    if(!load("cube.obj", model))
        return -1;
    
    fpsTimer.start();
    
    zBuffer.resize(surface->w * surface->h);
    std::fill(zBuffer.begin(), zBuffer.end(), -std::numeric_limits<float>::max());

    Vec3 translate =  {surface->w/2, surface->h/2, 0};
    Vec3 scale = {surface->w/2, surface->h/2, 1};

    const float aspectRatio = width / (float)height;
    mat4x4 viewMatrix = lookAt(Vec3{0 , 0, 1}, Vec3{0, 0, 0}, Vec3{0, 1, 0});
    mat4x4 perspective = setProjectionMatrix(90.f, 0.1f, 100.f, aspectRatio);
    mat4x4 viewportTransform = loadScale(scale) * loadTranslation(translate);

    while(Running) {
		while(SDL_PollEvent(&event)) {
        	if (event.type == SDL_QUIT) {
				printf("SDL_QUIT\n");
			    Running = false;
        	}

        	if ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE)) {
        	    printf("ESCAPE!\n");
				Running = false;
        	}
		}

        SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 255));

        std::fill(zBuffer.begin(), zBuffer.end(), -std::numeric_limits<float>::max());
        if(surface->w != width || surface->h != height){
            zBuffer.resize(surface->w * surface->h);
            std::fill(zBuffer.begin(), zBuffer.end(), -std::numeric_limits<float>::max());
            width = surface->w;
            height = surface->h;
        }

        int x, y;
        SDL_GetMouseState(&x, &y);
      //  printf("X=%d Y=%d\n",x,y);
        float mx, my;
        mx = (float)-1.f*(x / (surface->w / 2.f) - 1.f);
        my = (float)(y / (surface->h / 2.f) - 1.f);


//        Vec3 lightDir = {mx + 1, my + 1, -1.f};
        Vec3 lightDir = {mx, my, -1.f};
        for(uint32_t i = 0; i < model.faces.size(); i++) {
            VertexCoords vertices = grabTriVertexCoord(model, model.faces[i]);
            TriangleTextCoords verTextureCoords = grabTriTextCoord(model, model.faces[i]);

            Vertex screenCoords[3];

            screenCoords[0].pos.xyz = vertices.first  * viewMatrix * perspective * viewportTransform; //simplePerspective(vertices.first ) * viewportTransform;//simplePerspective(vertices.first )
            screenCoords[1].pos.xyz = vertices.second * viewMatrix * perspective * viewportTransform; //simplePerspective(vertices.second) * viewportTransform;//simplePerspective(vertices.second)
            screenCoords[2].pos.xyz = vertices.third  * viewMatrix * perspective * viewportTransform; //simplePerspective(vertices.third ) * viewportTransform;//simplePerspective(vertices.third )

            screenCoords[0].texCoords = verTextureCoords.first; 
            screenCoords[1].texCoords = verTextureCoords.second; 
            screenCoords[2].texCoords = verTextureCoords.third; 

            uint8_t c = i;
            Vec3 firstEdge = vertices.third - vertices.first;
            Vec3 secondEdge = vertices.second - vertices.first;
            Vec3 faceNormal = cross(firstEdge, secondEdge);

            faceNormal = normaliseVec3(faceNormal);
            float intensity = dotVec3(lightDir, faceNormal);
            if(intensity > 1)
                intensity = 1;
            if(intensity > 0) {
                drawTriangleHalfSpace(surface,
                    screenCoords[0], screenCoords[1], screenCoords[2],
                    Color{intensity, intensity, intensity, 255}
                    //Color{intensity * 255, intensity * 255, intensity * 255, 255}
                );
            }
        }
		int err = SDL_UpdateWindowSurface(window);
		FPSCounter++;
        assert(err == 0);
        //printf("Frame took %f [ms]\n",timer.stopMs());

        double elapsedTime = fpsTimer.stopMs();
        if(elapsedTime > 1000) {
            printf("AVG FPS = %f\n", 1000/(elapsedTime/FPSCounter));
            fpsTimer.start();
            FPSCounter = 0;
        }
	}
  	
  	SDL_DestroyWindow(window);
  	SDL_Quit();

    return 0;
}