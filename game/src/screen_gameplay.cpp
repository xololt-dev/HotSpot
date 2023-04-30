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

#include <algorithm>
#include <vector>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

struct Projectile {
    Vector2 position;
    Vector2 direction;
    float radius;
};

Camera2D camera;

Rectangle player, dummy;
Rectangle background;

std::vector<Projectile> projectiles;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here
    framesCounter = 0;
    finishScreen = 0;
    
    camera = Camera2D{ {0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f};
    player = Rectangle{ 400, 200, 20, 20 };
    dummy = Rectangle{ 600, 250, 20, 20 };
    background = Rectangle{ 0, 0, 800, 450 };
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

    // Projectiles update
    std::vector<Projectile>::iterator it;
    for (int i = 0; i < projectiles.size(); i++) {     
        projectiles[i].position = Vector2Add(projectiles[i].position, projectiles[i].direction);
    }
    for (int i = 0; i < projectiles.size(); i++) {
        if (0 - projectiles[i].radius > projectiles[i].position.x) {
            it = projectiles.begin() + i;
            projectiles.erase(it);
            i--;
        }
        else if (projectiles[i].position.x > 800 + projectiles[i].radius) {
            it = projectiles.begin() + i;
            projectiles.erase(it);
            i--;
        }
        else if (0 - projectiles[i].radius > projectiles[i].position.y) {
            it = projectiles.begin() + i;
            projectiles.erase(it);
            i--;
        }
        else if (projectiles[i].position.y > 450 + projectiles[i].radius) {
            it = projectiles.begin() + i;
            projectiles.erase(it);
            i--;
        }
    }
    float offsetX = 0, offsetY = 0;
    // needs fixing
    for (int i = 0; i < projectiles.size(); i++) {
        if (CheckCollisionCircleRec(projectiles[i].position, projectiles[i].radius, dummy)) {
            if ((projectiles[i].position.y >= (dummy.y + dummy.height)) || projectiles[i].position.y <= dummy.y) {
                offsetY += projectiles[i].direction.y * 0.1;
                projectiles[i].direction.y *= -1;
            }
            else if (projectiles[i].position.x >= (dummy.x + dummy.width) || projectiles[i].position.x <= dummy.x){
                offsetX += projectiles[i].direction.x * 0.1;
                projectiles[i].direction.x *= -1;
            }
            else {
                offsetY += projectiles[i].direction.y * 0.07;
                offsetX += projectiles[i].direction.x * 0.07;
                projectiles[i].direction = Vector2Negate(projectiles[i].direction);
            }
        }
    }
    dummy.x += offsetX;
    dummy.y += offsetY;

    if (IsKeyDown(KEY_W)) player.y -= 1;
    if (IsKeyDown(KEY_S)) player.y += 1;
    if (IsKeyDown(KEY_A)) player.x -= 1;
    if (IsKeyDown(KEY_D)) player.x += 1;

    // Generate projectile
    if (IsKeyDown(KEY_E)) {
        Projectile temp;
        temp.position = { player.x + player.width/2, player.y + player.height/2 };
        temp.direction = Vector2Normalize(Vector2Subtract(GetMousePosition(), temp.position));
        temp.radius = 1.0f;

        projectiles.push_back(temp);
    }
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void)
{
    // TODO: Draw GAMEPLAY screen here!
    BeginMode2D(camera);
    
        DrawRectangleRec(background, LIGHTGRAY);
        DrawRectangleRec(player, GOLD);
        DrawRectangleRec(dummy, BROWN);
        DrawRectangleLinesEx(background, 10.0f, BLACK);

        for (int i = 0; i < projectiles.size(); i++) {
            DrawCircleV(projectiles[i].position, projectiles[i].radius, RED);                
        }

    EndMode2D();
    DrawText(TextFormat("Vector size: %04i", projectiles.size()), 400, 20, 20, GREEN);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
    // TODO: Unload GAMEPLAY screen variables here!
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
    return finishScreen;
}