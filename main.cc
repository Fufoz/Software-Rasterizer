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

    Camera cam;
    cam.camPos  = Vec3{0.f, 0.f, 3.f};
    cam.forward = Vec3{0.f, 0.f, -1.f};
    cam.up      = Vec3{0.f, 1.f, 0.f};
    cam.pType   = PROJ_PERSPECTIVE;
    
    Vec4 clrColor = Vec4{88.f, 93.f, 102.f, 255.f};
    mat4x4 perspective = perspectiveProjection(60.f, context.window.width / context.window.height, 0.1f, 100.f);
    mat4x4 viewPort = viewport(context.window.width, context.window.height);
    setRenderState(viewPort, perspective, clrColor, cam);

    RenderObject cube1 = {};
    RenderObject cube2 = {};

    Texture cubeTexture = {};
    Mesh cubeMesh = {};
    if(!loadTexture("./resources/Bricks23_col.jpg", &cubeTexture))
        return -1;
    if(!loadMesh("./resources/cat.obj", &cubeMesh))
        return -1;

    cube1.mesh = &cubeMesh;
    cube1.texture = &cubeTexture;
    cube1.transform.scale = Vec3{0.01f, 0.01f, 0.01f};
    cube1.transform.translate = Vec3{0.f, 0.f, 0.f};
    //cube1.flatColor = {93, 150, 240};
    cube1.mode = MODE_FLATCOLOR;

    //cube2.mesh = &cubeMesh;
    //cube2.texture = &cubeTexture;
    //cube2.transform.scale = Vec3{1.5f, 1.5f, 1.5f};
    //cube2.transform.translate = Vec3{0.f, 0.f, 5.f};
    //cube2.flatColor = {250, 157, 7};
    //cube2.mode = MODE_FLATCOLOR;


    Timer tick = {};

    while(!windowClosed()) {
        tick.start();

        pollEvents();

        beginFrame(&context);
            renderObject(&context, cube1);
            //renderObject(&context, cube2);
        endFrame(&context);

//        printf("Frame took %.2f[ms] \n",tick.stopMs());
    }

    unloadTexture(cubeTexture.data);
    destroySoftwareRenderer(&context);
    return 0;
}