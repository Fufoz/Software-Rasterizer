#include <stdio.h>
#include <algorithm>

#include <limits>
#include "obj.h"
#include "timer.h"
#include "maths.h"
#include "input.h"
#include "renderer.h"
#include "camera.h"

int main(int argc, char **argv)
{
    RenderContext ctx = {};
    if(!createSoftwareRenderer(&ctx,"software renderer", 640, 480))
        return -1;

    Camera camera;
    camera.camPos  = Vec3{0.f, 0.f, 3.f};

    Vec4 clrColor = Vec4{88.f, 93.f, 102.f, 255.f};
    mat4x4 perspective = perspectiveProjection(60.f, ctx.window.width / ctx.window.height, 0.1f, 100.f);
    mat4x4 viewPort = viewport(ctx.window.width, ctx.window.height);
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
    cube1.transform.translate = Vec3{0.f, 0.f, -2.f};
    cube1.flatColor = {255, 0, 0};
    cube1.mode = MODE_FLATCOLOR;

    BumpShader shader = {};
    shader.sampler2d = &cubeTexture;
    shader.sampler2dD = &heightMap;
    shader.sampler2dN = &normalMap;
    
    Timer tick = {};
    double deltaTime = 0.f;
    while(!windowClosed()) {
        tick.start();

        pollEvents();
        updateCameraPosition(&camera, deltaTime);

        beginFrame(&ctx);
            renderObject(&ctx, cube1, camera, shader);
        endFrame(&ctx);
        deltaTime = tick.stopMs();
        //printf("Frame took %.2f[ms] \n",deltaTime);
    }

    unloadTexture(cubeTexture.data);
    destroySoftwareRenderer(&ctx);
    return 0;
}