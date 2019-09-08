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
    if(!createSoftwareRenderer(&ctx,"software renderer", 1080, 720))
        return -1;

    Camera camera = {};
    camera.camPos  = Vec3{0.f, 0.f, 3.f};

    Vec4 clrColor = Vec4{88.f, 93.f, 102.f, 255.f};
    mat4x4 perspective = perspectiveProjection(60.f, ctx.window.width / ctx.window.height, 0.1f, 10.f);
    mat4x4 viewPort = viewport(ctx.window.width, ctx.window.height);
    setRenderState(viewPort, perspective, clrColor);

    Mesh monkeyMesh = {};
    if(!loadMesh("./resources/monkey.obj", &monkeyMesh))
        return -1;

    FlatShader shader = {};
    shader.uniforms.in_flatColor = {32.f, 64.f, 123.f};

    RenderObject monkey1 = {};
    monkey1.mesh = &monkeyMesh;
    monkey1.transform.scale = Vec3{0.5f, 0.5f, 0.5f};
    monkey1.transform.translate = Vec3{0.f, 0.f, -1.f};

    Timer tick = {};
    double deltaTime = 0.f;
    while(!windowClosed()) {
        tick.start();

        pollEvents();
        updateCameraPosition(&camera, deltaTime);

        beginFrame(&ctx);
            renderObject(&ctx, monkey1, camera, shader);
        endFrame(&ctx);
        deltaTime = tick.stopMs();
        printf("Frame took %.2f[ms] \n",deltaTime);
    }

    destroySoftwareRenderer(&ctx);
    return 0;
}