#ifndef CAMERA_H
#define CAMERA_H

#include "math.h"
#include "input.h"

//#define cameraSpeed 0.05f

enum ProjectionType
{
    PROJ_PERSPECTIVE,
    PROJ_ORTHOGRAPHIC
};

struct Camera
{
    Vec3 up = {0.f, 1.f, 0.f};
    Vec3 forward = {0.f, 0.f, -1.f};
    Vec3 camPos;
    ProjectionType pType;
    mat4x4 worldToCameraTransform;
};

inline void updateCameraPosition(Camera* camera, double deltaTime)
{
    const float cameraSpeed = deltaTime * 0.0005f;
    static float pitch = 0.f;
    static float yaw = 0.f;

    if(isKeyPressed(BTN_W))//forward
        camera->camPos += cameraSpeed * camera->forward;
    if(isKeyPressed(BTN_A)) // strafe left
       camera->camPos -= cameraSpeed * normaliseVec3(cross(camera->forward, camera->up));
    if(isKeyPressed(BTN_S))//back
        camera->camPos -= cameraSpeed * camera->forward;
    if(isKeyPressed(BTN_D)) //strafe right
        camera->camPos += cameraSpeed * normaliseVec3(cross(camera->forward, camera->up));
    if(isKeyPressed(BTN_SPACE)) //up
        camera->camPos += cameraSpeed * camera->up;
    if(isKeyPressed(BTN_CTRL)) //down
        camera->camPos -= cameraSpeed * camera->up;
    
    Vec2 mouseDelta = getDeltaMousePosition();
    yaw += mouseDelta.x * -0.5f;
    pitch += mouseDelta.y * -0.5f;
/*    
    camera->forward.x = cos(toRad(yaw)) * cos(toRad(pitch));
    camera->forward.y = sin(toRad(pitch));
    camera->forward.z = sin(toRad(yaw)) * cos(toRad(pitch));
    camera->forward = normaliseVec3(camera->forward);

    camera->worldToCameraTransform = lookAt(camera->camPos, camera->camPos + camera->forward);
*/
    Quat qPitch = quatFromAxisAndAngle(Vec3{1, 0, 0}, pitch);
    Quat qYaw = quatFromAxisAndAngle(Vec3{0, 1, 0}, yaw);
    Quat orientation = qPitch * qYaw;
    Quat forwardQuat = {camera->forward.x, camera->forward.y, camera->forward.z, 0.f};
    Quat updatedForward = orientation * forwardQuat * conjugate(orientation);
    //camera->forward = normaliseVec3(updatedForward.complex);
    camera->worldToCameraTransform = lookAt(camera->camPos, camera->camPos + updatedForward.complex);
}
#endif