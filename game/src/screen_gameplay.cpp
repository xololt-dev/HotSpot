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

// black
// white
// #bd9b57
// #da2334

#include "raylib.h"
#include "raymath.h"
#include "rcamera.h"

#include "screens.hpp"
#include "camera.hpp"

#include <algorithm>
#include <vector>
#include <chrono>

#include <iostream>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static int screenHeight = 0;
static int screenWidth = 0;

static float diagonal = 0.1f / (float)sqrt(2);

struct Projectile {
	Vector2 position;
	Vector2 direction;
	float radius;
};

struct Entity {
	Rectangle hitbox;
	Vector2 direction;
	short health;
	short resource;
};

bool hasMoved = 0;
bool dummyHit = 0;

Camera2D camera;

Entity player1;
Texture2D playerTex;
Rectangle player, dummy;
Rectangle background, pillar;
BoundingBox dummyBB, pillarBB;

std::vector<Projectile> projectiles;
std::vector<Rectangle> hitboxes;		// because boundingBoxes seem to scale badly for 2D, to be used later
std::vector<BoundingBox> boundingBoxes;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

void drawShadowLines(Vector2* playerPos, Rectangle* object, Color color);

// Gameplay Screen Initialization logic
void InitGameplayScreen(void) {
	// TODO: Initialize GAMEPLAY screen variables here
	framesCounter = 0;
	finishScreen = 0;
	// SetTargetFPS(9999);
	playerTex = LoadTexture("resources/textures/Sprite-0005.png");

	screenHeight = GetRenderHeight();
	screenWidth = GetRenderWidth();
	
	camera = Camera2D{ {0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f };
	player = Rectangle{ 0.5f * screenWidth, 4.0f / 9.0f * screenHeight, 32.0f, 32.0f };
	player1 = {
		player,
		Vector2{1.0f, 0.0f},
		100,
		100
	};
	dummy = Rectangle{ 0.75f * screenWidth, 5.0f / 9.0f * screenHeight, 20.0f, 20.0f };
	background = Rectangle{ 0.0f, 0.0f, float(screenWidth), float(screenHeight) };
	pillar = Rectangle{ 0.125f * screenWidth, 0.125f * screenHeight, 50.0f, 50.0f };
	
	boundingBoxes.push_back(
		BoundingBox{ { dummy.x, dummy.y, -1.0f }, { dummy.x + dummy.width, dummy.y + dummy.height, 1.0f } });
	boundingBoxes.push_back(
		BoundingBox{ { pillar.x, pillar.y, -1.0f } ,{ pillar.x + pillar.width, pillar.y + pillar.height, 1.0f } });
	boundingBoxes.push_back(
		BoundingBox{ { background.x, background.y, -1.0f } , { background.x + background.width, background.y + 10.0f, 1.0f } });
	boundingBoxes.push_back(
		BoundingBox{ { background.x, background.y, -1.0f } , { background.x + 10.0f, background.y + background.height, 1.0f } });
	boundingBoxes.push_back(
		BoundingBox{ { background.x + background.width - 10.0f, background.y, 1.0f } , { background.x + background.width, background.y + background.height, -1.0f } });
	boundingBoxes.push_back(
		BoundingBox{ { background.x, background.y + background.height - 10.0f, 1.0f } , { background.x + background.width, background.y + background.height, -1.0f } });
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void) {
	// TODO: Update GAMEPLAY screen variables here!
	hasMoved = 0;
	dummyHit = 0;

	// Press enter or tap to change to ENDING screen
	if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
		finishScreen = 1;
		PlaySound(fxCoin);
		return;
	}

	if (IsWindowResized()) {
		screenWidth = GetScreenWidth();
		screenHeight = GetScreenHeight();
		background = Rectangle{ 0, 0, float(screenWidth), float(screenHeight) };

		for (short i = 0; i < 4; i++) boundingBoxes.pop_back();
		boundingBoxes.push_back(
			BoundingBox{ { background.x, background.y, -1.0f } , { background.x + background.width, background.y + 10.0f, 1.0f } });
		boundingBoxes.push_back(
			BoundingBox{ { background.x, background.y, -1.0f } , { background.x + 10.0f, background.y + background.height, 1.0f } });
		boundingBoxes.push_back(
			BoundingBox{ { background.x + background.width - 10.0f, background.y, 1.0f } , { background.x + background.width, background.y + background.height, -1.0f } });
		boundingBoxes.push_back(
			BoundingBox{ { background.x, background.y + background.height - 10.0f, 1.0f } , { background.x + background.width, background.y + background.height, -1.0f } });

		hasMoved = 1;
	}

	float offsetX = 0.0f, offsetY = 0.0f;
	// Projectiles update
	if (projectiles.size() > 0) {
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
		
		// needs fixing
		for (int i = 0; i < projectiles.size(); i++) {
			if (CheckCollisionCircleRec(projectiles[i].position, projectiles[i].radius, dummy)) {
				if ((projectiles[i].position.y >= (dummy.y + dummy.height)) || projectiles[i].position.y <= dummy.y) {
					offsetY += projectiles[i].direction.y * 0.1f;
					projectiles[i].direction.y *= -1;
				}
				else if (projectiles[i].position.x >= (dummy.x + dummy.width) || projectiles[i].position.x <= dummy.x) {
					offsetX += projectiles[i].direction.x * 0.1f;
					projectiles[i].direction.x *= -1;
				}
				else {
					offsetY += projectiles[i].direction.y * diagonal;
					offsetX += projectiles[i].direction.x * diagonal;
					projectiles[i].direction = Vector2Negate(projectiles[i].direction);
				}
			}
		}
		dummy.x += offsetX;
		dummy.y += offsetY;
		boundingBoxes[0].min.x += offsetX;
		boundingBoxes[0].min.y += offsetY;
		boundingBoxes[0].max.x += offsetX;
		boundingBoxes[0].max.y += offsetY;
	}	
	
	offsetX = offsetY = 0.0f;
	if (IsKeyDown(KEY_W)) {
		offsetY -= 1.0f;
		hasMoved = 1;
	}
	if (IsKeyDown(KEY_S)) {
		offsetY += 1.0f;
		hasMoved = 1;
	}
	if (IsKeyDown(KEY_A)) {
		offsetX -= 1.0f;
		hasMoved = 1;
	}
	if (IsKeyDown(KEY_D)) {
		offsetX += 1.0f;
		hasMoved = 1;
	}

	// punching
	if (IsKeyPressed(KEY_F)) {
		// check if player is looking in any direction
		if (!Vector2Equals(player1.direction, Vector2{ 0.0f, 0.0f })) {
			// distance between player and dummy
			Vector2 distanceV = Vector2Subtract({ player1.hitbox.x, player1.hitbox.y }, { dummy.x, dummy.y });
			Vector2 multiplyResult = Vector2Multiply(distanceV, player1.direction);
			distanceV = Vector2Normalize(Vector2Negate(distanceV));
			
			// if multiplication result is sub 0, it means that dummy is not hit by a punch
			if (multiplyResult.x < 0 || multiplyResult.y < 0) {
				dummy.x += distanceV.x;
				dummy.y += distanceV.y;
				boundingBoxes[0].min.x += distanceV.x;
				boundingBoxes[0].min.y += distanceV.y;
				boundingBoxes[0].max.x += distanceV.x;
				boundingBoxes[0].max.y += distanceV.y;

				dummyHit = 1;
			}
		}
	}

	if (hasMoved) {
		player1.hitbox.x += offsetX;
		player1.hitbox.y += offsetY;
		player1.direction = { offsetX, offsetY };
	}

	// Generate projectile
	if (IsKeyDown(KEY_E)) {
		Projectile temp;
		temp.position = { player1.hitbox.x + player1.hitbox.width / 2.0f, player1.hitbox.y + player1.hitbox.height / 2.0f };
		temp.direction = Vector2Normalize(Vector2Subtract(GetMousePosition(), temp.position));
		temp.radius = 1.0f;

		projectiles.push_back(temp);
	}
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void) {
	// TODO: Draw GAMEPLAY screen here!
	
	BeginMode2D(camera);
		ClearBackground(BLACK);

		Color bouncingColor = Color{ 245, 245, 245, 255 };
		// 170, 74, 68

		Vector2 playerPos = { player1.hitbox.x + player1.hitbox.width / 2.0f, player1.hitbox.y + player1.hitbox.height / 2.0f };
		DrawTexture(playerTex, player1.hitbox.x, player1.hitbox.y, WHITE);
		DrawLineV(playerPos, Vector2Add(playerPos, Vector2Scale(player1.direction,	20.0f)), GREEN);
		
		if (dummyHit) DrawRectangleRec(dummy, RED);
		else DrawRectangleRec(dummy, BROWN);

		DrawRectangleLinesEx(background, 10.0f, WHITE);
		DrawRectangleRec(pillar, WHITE);
		
		for (int i = 0; i < projectiles.size(); i++) {
			DrawCircleV(projectiles[i].position, projectiles[i].radius, RED);                
		}
		
		float healthFillup = player1.hitbox.width * (player1.health / 100.0f);
		DrawRectangle(player1.hitbox.x, player1.hitbox.y - 7.5f, healthFillup, 5.0f, RED);
		DrawRectangle(player1.hitbox.x + healthFillup, player1.hitbox.y - 7.5f, player1.hitbox.width - healthFillup, 5.0f, BLACK);

		drawShadowLines(&playerPos, &pillar, BLUE);
		drawShadowLines(&playerPos, &dummy, YELLOW);

	EndMode2D();
	
	DrawText(TextFormat("Vector size: %i", projectiles.size()), screenWidth / 2.0f, 20, 20, RED);
	/*
	DrawText(TextFormat("Boxy: %f", collisionBoxyCzas.count()), 400, 80, 20, RED);
	DrawText(TextFormat("Linie: %f", linieCzas.count()), 400, 100, 20, RED);
	DrawText(TextFormat("Hybryda: %f", hybridCzas.count()), 400, 120, 20, RED);
	*/
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {
	// TODO: Unload GAMEPLAY screen variables here!
	boundingBoxes.clear();

	UnloadTexture(playerTex);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) {
	return finishScreen;
}

void drawShadowLines(Vector2* playerPos, Rectangle* object, Color color) {
	Vector2 cornerPos = { object->x, object->y + object->height };
	Vector2 currentPos = cornerPos;
	float currentHigh = Vector2Distance(*playerPos, cornerPos);
	float distance = 0;

	for (int i = 1; i < 4; i++) {
		cornerPos = { object->x + object->width * (i > 1), object->y + object->height * !(i % 2) };
		distance = Vector2Distance(*playerPos, cornerPos);

		if (distance > currentHigh) {
			currentHigh = distance;
			DrawLineV(*playerPos, currentPos, color);
			currentPos = cornerPos;
		}
		else DrawLineV(*playerPos, cornerPos, color);
	}
}