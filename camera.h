#ifndef CAMERA_H
#define CAMERA_H

#include "math.h"
#include "input.h"

#define CAMERA_SPEED 0.05f

enum ProjectionType
{
    PROJ_PERSPECTIVE,
    PROJ_ORTHOGRAPHIC
};
float pitch = 0.f;
float yaw = -90.f;

struct Camera
{
    Vec3 camPos;
    Vec3 forward;
    Vec3 up;
    ProjectionType pType;
};

inline void updateCameraPosition(Camera* camera)
{
    if(isKeyPressed(BTN_W))//forward
        camera->camPos += CAMERA_SPEED * camera->forward;
    if(isKeyPressed(BTN_A)) // strafe left
       camera->camPos -= CAMERA_SPEED * cross(camera->forward, camera->up);
    if(isKeyPressed(BTN_S))//back
        camera->camPos -= CAMERA_SPEED * camera->forward;
    if(isKeyPressed(BTN_D)) //strafe right
        camera->camPos += CAMERA_SPEED * cross(camera->forward, camera->up);
    if(isKeyPressed(BTN_SPACE)) //up
        camera->camPos += CAMERA_SPEED * camera->up;
    if(isKeyPressed(BTN_CTRL)) //down
        camera->camPos -= CAMERA_SPEED * camera->up;

    
    
    Vec2 mouseDelta = getDeltaMousePosition();
    
    yaw += mouseDelta.x * 0.5f;
    pitch += mouseDelta.y * -0.5f;

    // make sure that when pitch is out of bounds, screen doesn't get flipped
    if (pitch > 89.0f)
        pitch = 89.0f;
    if (pitch < -89.0f)
        pitch = -89.0f;

    auto toRad = [](float degree) {return degree * PI / 180.f;};

    camera->forward.x = cos(toRad(yaw)) * cos(toRad(pitch));
    camera->forward.y = sin(toRad(pitch));
    camera->forward.z = sin(toRad(yaw)) * cos(toRad(pitch));
    
    camera->forward = normaliseVec3(camera->forward);

   // printf("forward {%f, %f, %f} CamPos {%f, %f, %f}\n",
   //     camera->forward.x,camera->forward.y,camera->forward.z,
   //     camera->camPos.x, camera->camPos.y, camera->camPos.z);
    //if(isMouseBtnPressed(MBTN_LEFT)){
    //    camera->forward = normaliseVec3(camera->forward * rotateY(-0.5f * mouseDelta.x));
    //    camera->forward = normaliseVec3(camera->forward * rotateX(-0.5f * mouseDelta.y));
    //    logMat4x4("Xr",rotateX(-0.5f * mouseDelta.y));
    //    camera->forward = normaliseVec3(camera->forward);
    //}
    
}
#endif