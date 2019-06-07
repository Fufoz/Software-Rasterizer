#include <stdio.h>
#include <algorithm>
#include "obj.h"
#include "timer.h"
#include <limits>
#include "math.h"
#include "input.h"
#include "renderer.h"


int main(int argc, char **argv)
{
    RenderContext context = {};
    if(!createSoftwareRenderer(&context,"software renderer", 640, 480))
        return -1;

    Target target = {};
    if(!load("./resources/rhscube.obj", target.mesh))
        return -1;
    if(!loadTexture("./resources/bricks.jpg", &(target.texture)))
        return -1;

    setViewPort(viewport(context.window.width, context.window.height));
    setClearColor(Vec4{88.f, 93.f, 102.f, 255.f});
    
    while(!windowClosed()) {
        pollEvents();
        beginFrame(&context);
        commitFrame(&context, target);
        endFrame(&context);
    }

    destroySoftwareRenderer(&context);
    return 0;
}