#include <stdio.h>
#include <algorithm>
#include "obj.h"
#include "timer.h"
#include <limits>
#include "math.h"
#include "input.h"
#include "renderer.h"
#include "camera.h"

int main(int argc, char **argv)
{
    RenderContext context = {};
    if(!createSoftwareRenderer(&context,"software renderer", 640, 480))
        return -1;

    setPerspective(perspectiveProjection(90.f, context.window.width / context.window.height, 0.1f, 100.f));    
    setViewPort(viewport(context.window.width, context.window.height));
    setClearColor(Vec4{88.f, 93.f, 102.f, 255.f});//grayish

    Camera camera;
    camera.camPos  = Vec3{0.f, 0.f, 3.f};
    camera.forward = Vec3{0.f, 0.f, -1.f};
    camera.up      = Vec3{0.f, 1.f, 0.f};
    camera.pType   = PROJ_PERSPECTIVE;
    setCamera(camera);

    RenderObject cube1 = {};
    RenderObject cube2 = {};

    Texture cubeTexture = {};
    Mesh cubeMesh = {};
    if(!loadTexture("./resources/Bricks23_col.jpg", &cubeTexture))
        return -1;
    if(!loadMesh("./resources/texturedCube.obj", &cubeMesh))
        return -1;

    cube1.mesh = &cubeMesh;
    cube1.texture = &cubeTexture;
    cube1.transform.scale = Vec3{0.5f, 0.5f, 0.5f};
    cube1.transform.translate = Vec3{0.f, 0.f, 0.f};

    cube2.mesh = &cubeMesh;
    cube2.texture = &cubeTexture;
    cube2.transform.scale = Vec3{1.5f, 1.5f, 1.5f};
    cube2.transform.translate = Vec3{0.f, 0.f, 5.f};


    Timer tick = {};

    while(!windowClosed()) {
        tick.start();

        pollEvents();

        beginFrame(&context);
            renderObject(&context, cube1, RenderMode::MODE_TEXTURED);
            renderObject(&context, cube2, RenderMode::MODE_TEXTURED);
        endFrame(&context);

        printf("Frame took %.2f[ms] \n",tick.stopMs());
    }

    unloadTexture(cubeTexture.data);
    destroySoftwareRenderer(&context);
    return 0;
}