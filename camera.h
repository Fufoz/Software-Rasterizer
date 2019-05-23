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

struct Camera
{
    Vec3 camPos;
    Vec3 forward;
    Vec3 up;
    ProjectionType pType;
};

//float yaw = 0;
//float pitch = 0;

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
    camera->forward = normaliseVec3(camera->forward * rotateY(-0.5f * mouseDelta.x));
    camera->forward = normaliseVec3(camera->forward * rotateX(-0.5f * mouseDelta.y));
    camera->forward = normaliseVec3(camera->forward);


}
#endif