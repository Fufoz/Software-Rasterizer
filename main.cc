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

    Camera camera;
    camera.camPos  = Vec3{0.f, 0.f, 3.f};
    camera.forward = Vec3{0.f, 0.f, -1.f};
    camera.up      = Vec3{0.f, 1.f, 0.f};
    camera.pType   = PROJ_PERSPECTIVE;
    
    Vec4 clrColor = Vec4{88.f, 93.f, 102.f, 255.f};
    mat4x4 perspective = perspectiveProjection(60.f, context.window.width / context.window.height, 0.001f, 1000.f);
    mat4x4 viewPort = viewport(context.window.width, context.window.height);
    setRenderState(viewPort, perspective, clrColor);
    mat4x4 a = {
        23,4,3,2,
        12,4,2,4,
        12,52,7,4,
        1,2,1,7
    };

    RenderObject cube1 = {};
    RenderObject cube2 = {};

    Texture cubeTexture = {};
    Mesh cubeMesh = {};
    if(!loadTexture("./resources/Bricks23_col.jpg", &cubeTexture))
        return -1;
    if(!loadMesh("./resources/texturedCube.obj", &cubeMesh))
        return -1;
        
    averageNormals(&cubeMesh);
    cube1.mesh = &cubeMesh;
    cube1.texture = &cubeTexture;
    cube1.transform.scale = Vec3{0.5f, 0.5f, 0.5f};
    cube1.transform.translate = Vec3{0.f, 0.f, 0.f};
    cube1.flatColor = {93, 150, 240};
    cube1.mode = MODE_FLATCOLOR;

    cube2.mesh = &cubeMesh;
    cube2.texture = &cubeTexture;
    cube2.transform.scale = Vec3{1.5f, 1.5f, 1.5f};
    cube2.transform.translate = Vec3{0.f, 0.f, 5.f};
    cube2.flatColor = {250, 157, 7};
    cube2.mode = MODE_FLATCOLOR;


    Timer tick = {};
    double deltaTime = 0.f;
    while(!windowClosed()) {
        tick.start();

        pollEvents();
        updateCameraPosition(&camera, deltaTime);

        beginFrame(&context);
            renderObject(&context, cube1, camera);
            renderObject(&context, cube2, camera);
        endFrame(&context);
        deltaTime = tick.stopMs();
//        printf("Frame took %.2f[ms] \n",deltaTime);
    }

    unloadTexture(cubeTexture.data);
    destroySoftwareRenderer(&context);
    return 0;
}