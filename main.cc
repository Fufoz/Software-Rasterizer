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
    mat4x4 perspective = perspectiveProjection(60.f, context.window.width / context.window.height, 0.1f, 10.f);
    mat4x4 viewPort = viewport(context.window.width, context.window.height);
    setRenderState(viewPort, perspective, clrColor);


    RenderObject cube1 = {};
    Texture cubeTexture = {};
    Mesh cubeMesh = {};
    if(!loadMesh("./resources/planeZ.obj", &cubeMesh))
        return -1;
    if(!loadTexture("./resources/rough_block_wall_diff_2k.jpg", &cubeTexture))
        return -1;
    Texture normalMap = {};
    if(!loadTexture("./resources/rough_block_wall_nor_2k.jpg", &normalMap))
        return -1;

    Texture heightMap = {};
    if(!loadTexture("./resources/rough_block_wall_disp_2k.jpg", &heightMap))
        return -1;

    averageNormals(&cubeMesh);
    fillTangent(&cubeMesh);
    cube1.mesh = &cubeMesh;
    cube1.texture = &cubeTexture;
    cube1.normalMap = &normalMap;
    cube1.heightMap = &heightMap;
    cube1.transform.scale = Vec3{0.1f, 0.1f, 0.1f};
    cube1.transform.translate = Vec3{0.f, 0.f, -3.f};
    cube1.flatColor = {255, 0, 0};
    cube1.mode = MODE_FLATCOLOR;

    Timer tick = {};
    double deltaTime = 0.f;
    while(!windowClosed()) {
        tick.start();

        pollEvents();
        updateCameraPosition(&camera, deltaTime);

        beginFrame(&context);
            renderObject(&context, cube1, camera);
        endFrame(&context);
        deltaTime = tick.stopMs();
        printf("Frame took %.2f[ms] \n",deltaTime);
    }

    unloadTexture(cubeTexture.data);
    destroySoftwareRenderer(&context);
    return 0;
}