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
    if(!load("./resources/rhscube.obj", target.mesh))
        return -1;
    if(!loadTexture("./resources/bricks.jpg", &(target.texture)))
        return -1;
        
    while(context.isRunning) {
        processInput(&context);
        beginFrame(&context);
        commitFrame(&context, target);
        endFrame(&context);
    }

    destroySoftwareRenderer(&context);
    return 0;
}