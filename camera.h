#ifndef CAMERA_H
#define CAMERA_H

#include "math.h"
#include "input.h"

enum ProjectionType
{
    PROJ_PERSPECTIVE,
    PROJ_ORTHOGRAPHIC
};

struct Camera
{
    Vec3 camPos;
    Vec3 up = {0.f, 1.f, 0.f};
    Vec3 forward = {0.f, 0.f, -1.f};
    ProjectionType pType;
    mat4x4 worldToCameraTransform;
};

inline void updateCameraPosition(Camera* camera, double deltaTime)
{
    const float cameraSpeed = deltaTime * 0.005f;
    static float pitch = 0.f;
    static float yaw = -90.f;

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
    yaw += mouseDelta.x * 0.5f;
    pitch += mouseDelta.y * -0.5f;

	//avoid scene flips at corner pitch angles 
    pitch = std::abs(pitch) > 89.f ? (pitch < 0 ? -89.f : 89.f) : pitch; 

    camera->forward.x = cos(toRad(yaw)) * cos(toRad(pitch));
    camera->forward.y = sin(toRad(pitch));
    camera->forward.z = sin(toRad(yaw)) * cos(toRad(pitch));
	printf("FWD: %f %f %f\n",camera->forward.x, camera->forward.y, camera->forward.z);
	printf("Pitch %f\n", pitch);
    camera->forward = normaliseVec3(camera->forward);
	camera->up = normaliseVec3(camera->up * rotateY(pitch));
    camera->worldToCameraTransform = lookAt(camera->camPos, camera->camPos + camera->forward, camera->up);
/*
    Quat qPitch = quatFromAxisAndAngle(Vec3{1, 0, 0}, pitch);
    Quat qYaw = quatFromAxisAndAngle(Vec3{0, 1, 0}, yaw);
    Quat orientation = qPitch * qYaw;
    Quat forwardQuat = {camera->forward.x, camera->forward.y, camera->forward.z, 0.f};
    Quat updatedForward = orientation * forwardQuat * conjugate(orientation);
    //camera->forward = normaliseVec3(updatedForward.complex);
    camera->worldToCameraTransform = lookAt(camera->camPos, camera->camPos + updatedForward.complex);
*/
}
#endif