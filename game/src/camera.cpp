#include "raylib.h"
#include "rcamera.h"
#include "camera.hpp"

void SetupCamera(Camera* camera) {
    camera->position = { 0.0f, 7.0f, 7.0f };             // Camera position
    camera->target = { 0.0f, 0.0f, 0.0f };               // Camera looking at point
    camera->up = { 0.0f, 1.0f, 0.0f };                   // Camera up vector (rotation towards target)
    camera->fovy = 60.0f;                                // Camera field-of-view Y
    camera->projection = CAMERA_PERSPECTIVE;             // Camera mode type
}

int UpdateCameraView(Camera *camera) {
    int cameraMode = 0;
    
    if (camera->projection == CAMERA_PERSPECTIVE)
    {
        // Create isometric view
        cameraMode = CAMERA_THIRD_PERSON;
        // Note: The target distance is related to the render distance in the orthographic projection
        camera->position = { 0.0f, 2.0f, 100.0f };
        camera->target = { 0.0f, 2.0f, 0.0f };
        camera->up = { 0.0f, 1.0f, 0.0f };
        camera->projection = CAMERA_ORTHOGRAPHIC;
        camera->fovy = 20.0f; // near plane width in CAMERA_ORTHOGRAPHIC
        CameraYaw(camera, -135 * DEG2RAD, true);
        CameraPitch(camera, -45 * DEG2RAD, true, true, false);
    }
    else if (camera->projection == CAMERA_ORTHOGRAPHIC)
    {
        // Reset to default view
        cameraMode = CAMERA_FIRST_PERSON;
        camera->position = { 0.0f, 2.0f, 10.0f };
        camera->target = { 0.0f, 2.0f, 0.0f };
        camera->up = { 0.0f, 1.0f, 0.0f };
        camera->projection = CAMERA_PERSPECTIVE;
        camera->fovy = 60.0f;
    }

    return cameraMode;
}