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
    Vec3 camPos;
    Vec3 forward;
    Vec3 up;
    ProjectionType pType;
    mat4x4 worldToCameraTransform;
};

inline void updateCameraPosition(Camera* camera, double deltaTime)
{
    const float cameraSpeed = deltaTime * 0.005f;
//    printf("Camera speed %f\n",cameraSpeed);
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

    //camera->up = 
    
    Vec2 mouseDelta = getDeltaMousePosition();
    yaw += mouseDelta.x * 0.5f;
    pitch += mouseDelta.y * -0.5f;
    
    if(std::abs(pitch) > 85.f) {
        pitch = pitch < 0 ? -85.f : 85.f;
    }
    
    camera->forward.x = cos(toRad(yaw)) * cos(toRad(pitch));
    camera->forward.y = sin(toRad(pitch));
    camera->forward.z = sin(toRad(yaw)) * cos(toRad(pitch));
	printf("FWD: %f %f %f\n",camera->forward.x, camera->forward.y, camera->forward.z);
	printf("Pitch %f\n", pitch);
    camera->forward = normaliseVec3(camera->forward);
	camera->up = normaliseVec3(camera->up * rotateY(pitch));
    camera->worldToCameraTransform = lookAt(camera->camPos, camera->camPos + camera->forward, camera->up);
}
#endif