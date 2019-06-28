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
    mat4x4 worldToCameraTransform;
};

inline void updateCameraPosition(Camera* camera)
{
    static float pitch = 0.f;
    static float yaw = -90.f;

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
    
    if(std::abs(pitch) > 85.f) {
        pitch = pitch < 0 ? -85.f : 85.f;
    }

    printf("YAW=%f PITCH = %f\n",yaw,pitch);    

    camera->forward.x = cos(toRad(yaw)) * cos(toRad(pitch));
    camera->forward.y = sin(toRad(pitch));
    camera->forward.z = sin(toRad(yaw)) * cos(toRad(pitch));
    
    camera->forward = normaliseVec3(camera->forward);
    camera->worldToCameraTransform = lookAt(camera->camPos, camera->camPos + camera->forward);    
}
#endif