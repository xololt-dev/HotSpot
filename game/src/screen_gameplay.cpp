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

static int screenHeight = 0;
static int screenWidth = 0;

struct Projectile {
    Vector2 position;
    Vector2 direction;
    float radius;
};

Camera2D camera;

Rectangle player, dummy;
Rectangle background, pillar;
BoundingBox dummyBB, pillarBB;

std::vector<Projectile> projectiles;
std::vector<Ray> rays;
std::vector<Ray> bounces;
std::vector<BoundingBox> boundingBoxes;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
    // TODO: Initialize GAMEPLAY screen variables here
    framesCounter = 0;
    finishScreen = 0;
    
    screenHeight = GetRenderHeight();
    screenWidth = GetRenderWidth();
    
    camera = Camera2D{ {0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f };
    player = Rectangle{ 400, 200, 20, 20 };
    dummy = Rectangle{ 600, 250, 20, 20 };
    background = Rectangle{ 0, 0, float(screenWidth), float(screenHeight) };
    pillar = Rectangle{ 100, 100, 50, 50 };

    for (int i = 0; i < 90; i++) {
        Vector2 temp = { 0.0f, -1.0f };
        temp = Vector2Rotate(temp, 4.0f * i * DEG2RAD);
        rays.push_back(Ray{ {400.0f + 10.0f, 200.f + 10.0f, 0.0f}, { temp.x, temp.y, 0.0f } });
    }

    boundingBoxes.push_back(
        BoundingBox{ { dummy.x, dummy.y, 0.0f }, { dummy.x + dummy.width, dummy.y + dummy.height, 0.0f } });
    boundingBoxes.push_back(
        BoundingBox{ { pillar.x, pillar.y, 0.0f } ,{ pillar.x + pillar.width, pillar.y + pillar.height, 0.0f } });
    boundingBoxes.push_back(
        BoundingBox{ { background.x, background.y, 0.0f } , { background.x + background.width, background.y + 10.0f, 0.0f } });
    boundingBoxes.push_back(
        BoundingBox{ { background.x, background.y, 0.0f } , { background.x + 10.0f, background.y + background.height, 0.0f } });
    boundingBoxes.push_back(
        BoundingBox{ { background.x + background.width - 10.0f, background.y, 0.0f } , { background.x + background.width, background.y + background.height, 0.0f } });
    boundingBoxes.push_back(
        BoundingBox{ { background.x, background.y + background.height - 10.0f, 0.0f } , { background.x + background.width, background.y + background.height, 0.0f } });
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
        else if (projectiles[i].position.x > screenWidth + projectiles[i].radius) {
            it = projectiles.begin() + i;
            projectiles.erase(it);
            i--;
        }
        else if (0 - projectiles[i].radius > projectiles[i].position.y) {
            it = projectiles.begin() + i;
            projectiles.erase(it);
            i--;
        }
        else if (projectiles[i].position.y > screenHeight + projectiles[i].radius) {
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
    
    if (IsKeyDown(KEY_W)) {
        player.y -= 1;
        for (int i = 0; i < rays.size(); i++) {
            rays[i].position.y -= 1;
        }
    }
    if (IsKeyDown(KEY_S)) {
        player.y += 1;
        for (int i = 0; i < rays.size(); i++) {
            rays[i].position.y += 1;
        }
    }
    if (IsKeyDown(KEY_A)) {
        player.x -= 1;
        for (int i = 0; i < rays.size(); i++) {
            rays[i].position.x -= 1;
        }
    }
    if (IsKeyDown(KEY_D)) {
        player.x += 1;
        for (int i = 0; i < rays.size(); i++) {
            rays[i].position.x += 1;
        }
    }

    // Generate ray bounces
    bounces.clear();

    for (int i = 0; i < rays.size(); i++) {
        RayCollision finalCollisionForRay = RayCollision{ 0, 0.0f, {0,0,0}, {0,0,0} };

        for (int j = 0; j < boundingBoxes.size(); j++) {
            RayCollision temp;
            temp = GetRayCollisionBox(rays[i], boundingBoxes[j]);

            if ((temp.hit && (temp.distance < finalCollisionForRay.distance)) || !finalCollisionForRay.hit) {
                finalCollisionForRay = temp;
            }
        }

        // reflection normals don't work in 2D (probably related to axis)
        if (finalCollisionForRay.hit) {
            Vector2 reflect = Vector2Reflect({ finalCollisionForRay.point.x, finalCollisionForRay.point.y }, { finalCollisionForRay.normal.x, finalCollisionForRay.normal.y });
            Vector3 directionReturn = Vector3{ reflect.x, reflect.y, 0.0f };
            bounces.push_back(
                Ray{ finalCollisionForRay.point, directionReturn});
            // bounces.push_back(
               // Ray{ finalCollisionForRay.point, finalCollisionForRay.normal});
            /*
            if ((rays[i].position.y >= finalCollisionForRay.point.y) || rays[i].position.y <= finalCollisionForRay.point.y) {
                bounces.push_back(Ray{ finalCollisionForRay.point, {rays[i].direction.x, rays[i].direction.y * -1.0f, rays[i].direction.z } });
            }
            else if (rays[i].position.x >= finalCollisionForRay.point.x || rays[i].position.x <= finalCollisionForRay.point.x) {
                bounces.push_back(Ray{ finalCollisionForRay.point, {rays[i].direction.x * -1.0f, rays[i].direction.y, rays[i].direction.z } });
            }
            else {
                bounces.push_back(Ray{ finalCollisionForRay.point, Vector3Negate(rays[i].direction) });
            }*/
        }
    }

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
        ClearBackground(BLACK);
        RayCollision temp, temp2;
        bool dummyHit = false;

        for (int i = 0; i < rays.size(); i++) {
            temp = GetRayCollisionBox(rays[i], boundingBoxes[0]);
            temp2 = GetRayCollisionBox(rays[i], boundingBoxes[1]);

            if (temp2.hit) {
                if (!temp.hit || (temp2.distance < temp.distance)) temp = temp2;
            }
            else {
                if (temp.hit) dummyHit = true;
            }
            // if (temp2.hit && (temp2.distance < temp.distance) || !temp.hit) temp = temp2;
            
            if (temp.hit) {
                DrawLineV({ rays[i].position.x, rays[i].position.y }, { temp.point.x, temp.point.y }, WHITE);
            }
            else DrawRay(rays[i], WHITE);
        }

        for (int i = 0; i < bounces.size(); i++) {
            temp = GetRayCollisionBox(bounces[i], boundingBoxes[0]);
            temp2 = GetRayCollisionBox(bounces[i], boundingBoxes[1]);

            if (temp2.hit) {
                if (!temp.hit || (temp2.distance < temp.distance)) temp = temp2;
            }
            else {
                if (temp.hit) dummyHit = true;
            }
            // if (temp2.hit && (temp2.distance < temp.distance) || !temp.hit) temp = temp2;

            if (temp.hit) {
                DrawLineV({ bounces[i].position.x, bounces[i].position.y }, { temp.point.x, temp.point.y }, BLUE);
            }
            else DrawRay(bounces[i], BLUE);
        }
    
        // DrawRectangleRec(background, LIGHTGRAY);
        DrawRectangleRec(player, GOLD);
        if (dummyHit) DrawRectangleRec(dummy, BROWN);
        DrawRectangleLinesEx(background, 10.0f, BLACK);
        DrawRectangleRec(pillar, BLACK);
        
        for (int i = 0; i < projectiles.size(); i++) {
            DrawCircleV(projectiles[i].position, projectiles[i].radius, RED);                
        }

    EndMode2D();
    
    DrawText(TextFormat("Vector size: %04i", projectiles.size()), 400, 20, 20, GREEN);
    //DrawText(TextFormat("Position: %03i, %03i", playerOne.x, playerOne.y), 400, 40, 20, GREEN);
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