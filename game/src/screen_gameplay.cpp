/**********************************************************************************************
*
*   raylib - Advance Game template
*
*   Gameplay Screen Functions Definitions (Init, Update, Draw, Unload)
*
*   Copyright (c) 2014-2022 Ramon Santamaria (@raysan5)
*
*   This software is provided "as-is", without any express or implied warranty. In no event
*   will the authors be held liable for any damages arising from the use of this software.
*
*   Permission is granted to anyone to use this software for any purpose, including commercial
*   applications, and to alter it and redistribute it freely, subject to the following restrictions:
*
*     1. The origin of this software must not be misrepresented; you must not claim that you
*     wrote the original software. If you use this software in a product, an acknowledgment
*     in the product documentation would be appreciated but is not required.
*
*     2. Altered source versions must be plainly marked as such, and must not be misrepresented
*     as being the original software.
*
*     3. This notice may not be removed or altered from any source distribution.
*
**********************************************************************************************/

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

#include "screens.hpp"
#include "camera.hpp"

#define GLSL_VERSION    330

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

// Define the camera to look into our 3d world
Camera3D camera = { 0 };
int cameraMode = CAMERA_FIRST_PERSON;

Vector3 cubePosition = { 0.0f, 1.0f, 0.0f };

Model model;
BoundingBox bounds;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here!
    framesCounter = 0;
    finishScreen = 0;

    SetupCamera(&camera);
    
    model = LoadModel("resources/models/icosphere.gltf");                   // Load model
    bounds = GetMeshBoundingBox(model.meshes[0]);                                   // Set model bounds
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
    // TODO: Update GAMEPLAY screen variables here!
    
    // Press enter or tap to change to ENDING screen
    if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
        finishScreen = 1;
        PlaySound(fxCoin);
    }

    // Switch camera projection
    if (IsKeyPressed(KEY_P)) cameraMode = UpdateCameraView(&camera);

    UpdateCamera(&camera, cameraMode);
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    
    BeginMode3D(camera);
        ClearBackground(RAYWHITE);
        
        DrawPlane({ 0.0f, 0.0f, 0.0f }, { 100.0f, 100.0f }, { 144, 238, 144, 255 });
        DrawSphere({ 0.0f, 0.0f, -75.0f }, 25.0f, YELLOW);
        
        DrawModel(model, cubePosition, 1.0f, LIGHTGRAY);

        DrawCube({ 0.0f, 2.05f, -3.0f }, 6.1f, 4.0f, 0.1f, RED);
        DrawCube({ -3.0f, 2.05f, 0.025f }, 0.1f, 4.0f, 5.95f, GREEN);
        DrawCube({ 3.0f, 2.05f, 0.025f }, 0.1f, 4.0f, 5.95f, BLUE);
        DrawCube({ 0.0f, 0.0f, 0.0f }, 6.1f, 0.1f, 6.1f, WHITE);
        DrawCube({ 0.0f, 4.1f, 0.0f }, 6.1f, 0.1f, 6.1f, BLACK);

        // DrawGrid(10, 1.0f);

    EndMode3D(); 
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
    UnloadModel(model);         // Unload model
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}