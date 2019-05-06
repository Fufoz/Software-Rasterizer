#include <stdio.h>
#include <algorithm>
#include "obj.h"
#include "timer.h"
#include <limits>
#include "mat.h"

#include "renderer.h"


int main(int argc, char **argv)
{
    RenderContext context = {};
    if(!createSoftwareRenderer(&context,"software renderer", 640, 480))
        return -1;

	SDL_Event event;
	uint32_t FPSCounter = 0;
    Timer fpsTimer;

    Target target = {};
    if(!load("./resources/cube.obj", target.mesh))
        return -1;
    if(!loadTexture("./resources/bricks.jpg", &(target.texture)))
        return -1;
        
    fpsTimer.start();
    
    Vec3 translate =  {context.surface->w/2 - 0.5f, context.surface->h/2 - 0.5f, 0};
    Vec3 scale = {context.surface->w/2, context.surface->h/2, 1};

    const float aspectRatio = context.width / (float)context.height;
    mat4x4 viewMatrix = lookAt(Vec3{0 , 0, 1}, Vec3{0, 0, 0}, Vec3{0, 1, 0});
    mat4x4 perspective = perspectiveProjection(90.f, aspectRatio, 0.1f, 100.f);

    while(context.isRunning) {
		while(SDL_PollEvent(&event)) {
        	if (event.type == SDL_QUIT) {
				printf("SDL_QUIT\n");
			    context.isRunning = false;
        	}

        	if ((event.type == SDL_KEYDOWN) && (event.key.keysym.sym == SDLK_ESCAPE)) {
        	    printf("ESCAPE!\n");
				context.isRunning = false;
        	}
		}
        beginFrame(&context);
        commitFrame(&context, target);
        endFrame(&context);
    }

/*
        SDL_FillRect(surface, NULL, SDL_MapRGBA(surface->format, 0, 0, 0, 255));

        std::fill(zBuffer.begin(), zBuffer.end(), -std::numeric_limits<float>::max());
        if(surface->w != width || surface->h != height) {
            zBuffer.resize(surface->w * surface->h);
            std::fill(zBuffer.begin(), zBuffer.end(), -std::numeric_limits<float>::max());
            width = surface->w;
            height = surface->h;
        }

        int x, y;
        SDL_GetMouseState(&x, &y);

        float mx, my;
        mx = (float)-1.f*(x / (surface->w / 2.f) - 1.f);
        my = (float)(y / (surface->h / 2.f) - 1.f);

        Vec3 lightDir = {mx, my, -1.f};
        for(uint32_t i = 0; i < model.faces.size(); i++) {
            VertexCoords vertices = grabTriVertexCoord(model, model.faces[i]);
            TriangleTextCoords verTextureCoords = grabTriTextCoord(model, model.faces[i]);

            Vertex screenCoords[3];

            screenCoords[0].pos = homogenize(vertices.first) * viewMatrix * simplePerspective(vertices.first);// * simplePerspective(vertices.first);//perspective;
            screenCoords[1].pos = homogenize(vertices.second)* viewMatrix * simplePerspective(vertices.second);// * simplePerspective(vertices.second);//perspective;
            screenCoords[2].pos = homogenize(vertices.third) * viewMatrix * simplePerspective(vertices.third);// * simplePerspective(vertices.third);//perspective;

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
                    Vec4{intensity, intensity, intensity, 255}
                );
            }
        }
		int err = SDL_UpdateWindowSurface(window);
		FPSCounter++;
        assert(err == 0);


        double elapsedTime = fpsTimer.stopMs();
        if(elapsedTime > 1000) {
            printf("AVG FPS = %f\n", 1000/(elapsedTime/FPSCounter));
            fpsTimer.start();
            FPSCounter = 0;
        }
	}
  	*/
    destroySoftwareRenderer(&context);
    return 0;
}