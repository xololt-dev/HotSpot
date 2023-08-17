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
#include <fstream>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static int screenHeight = 0;
static int screenWidth = 0;

struct Car {
	Rectangle hitbox;
	Vector2 position;
	Vector2 direction;
	float speed;
	float rotation;

	Texture2D apperance;

	float fuel;
};

struct Corner {
	Vector2 position;
	float width;
	float radius;
	float startAngle;
	float endAngle;
};

struct Track {
	std::vector<Corner> corners;
	std::vector<Rectangle> straights;
};

Camera2D camera;

Car player;
Track track;

Rectangle background;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------

// Racing top down
// ASM like upgrades
// Creating a vehicle
// Different tracks/planets(?)

void drawShadowLines(Vector2* playerPos, Rectangle* object, Color color);
void drawCorner(Corner* corner);

// Gameplay Screen Initialization logic
void InitGameplayScreen(void) {
	// TODO: Initialize GAMEPLAY screen variables here
	framesCounter = 0;
	finishScreen = 0;
	// SetTargetFPS(9999);

	screenHeight = GetRenderHeight();
	screenWidth = GetRenderWidth();
	
	std::fstream track1;
	track1.open("resources/tracks/track1.json", std::ios::in);
	if (track1.good()) {
		std::string tempS;
		int tempInt;

		while (tempS != "\"one\":" && !track1.eof()) {
			track1 >> tempS;
		}
		Corner tempCorner;

		for (int i = 0; i < 4; i++) {
			track1 >> tempS;
			track1 >> tempS;

			track1 >> tempCorner.position.x;
			track1 >> tempS;
			track1 >> tempS;
			track1 >> tempCorner.position.y;
			track1 >> tempS;
			track1 >> tempS;
			track1 >> tempCorner.width;
			track1 >> tempS;
			track1 >> tempS;
			track1 >> tempCorner.radius;
			track1 >> tempS;
			track1 >> tempS;
			track1 >> tempCorner.startAngle;
			track1 >> tempS;
			track1 >> tempS;
			track1 >> tempCorner.endAngle;

			track1 >> tempS;
			track1 >> tempS;

			track.corners.push_back(tempCorner);
		}

		while (tempS != "\"one\":" && !track1.eof()) {
			track1 >> tempS;
		}

		Rectangle tempRec;
		for (int i = 0; i < 4; i++) {
			track1 >> tempS;
			track1 >> tempS;

			track1 >> tempRec.x;
			track1 >> tempS;
			track1 >> tempS;
			track1 >> tempRec.y;
			track1 >> tempS;
			track1 >> tempS;
			track1 >> tempRec.width;
			track1 >> tempS;
			track1 >> tempS;
			track1 >> tempRec.height;

			track1 >> tempS;
			track1 >> tempS;

			track.straights.push_back(tempRec);
		}

		track1.close();
	}
	else std::cout << "Plik nie otworzony";

	camera = Camera2D { 
		{0.0f, 0.0f}, 
		{0.0f, 0.0f}, 
		0.0f, 
		1.0f 
	};
	player = Car {
		Rectangle { 
			0.5f * screenWidth, 
			4.0f / 9.0f * screenHeight, 
			32.0f, 
			32.0f 
		},
		Vector2 { 
			0.5f * screenWidth + 16.0f, 
			4.0f / 9.0f * screenHeight + 16.0f
		},
		Vector2 { 1.0f, 0.0f },
		0.0f,
		0.0f,
		LoadTexture("resources/textures/Sprite-0005.png"),
		100.0f
	};
	background = Rectangle { 
		0.0f, 
		0.0f,
		float(screenWidth),
		float(screenHeight)
	};
	/*
	track = Track {
		track.corners = {
			{
				{125.0f, 125.0f},
				75.0f,
				90.0f,
				180.0f,
				270.0f
			},
			{
				{325.0f, 325.0f},
				75.0f,
				90.0f,
				0.0f,
				90.0f
			},
			{
				{225.0f, 225.0f},
				75.0f,
				190.0f,
				90.0f,
				180.0f
			},
			{
				{225.0f, 225.0f},
				75.0f,
				190.0f,
				270.0f,
				360.0f
			},
		},
		track.straights = {
			{ 35.0f, 125.0f, 75.0f, 100.0f },
			{ 340.0f, 225.0f, 75.0f, 100.0f },
			{ 125.0f, 35.0f, 100.0f, 75.0f },
			{ 225.0f, 340.0f, 100.0f, 75.0f },
		}
	};*/
	/*
	track.straights = {
			{ 35.0f, 125.0f, 75.0f, 100.0f },
			{ 340.0f, 225.0f, 75.0f, 100.0f },
			{ 125.0f, 35.0f, 100.0f, 75.0f },
			{ 225.0f, 340.0f, 100.0f, 75.0f },
	};*/
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void) {
	// TODO: Update GAMEPLAY screen variables here!

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
	}
	
	/*
	float offsetX = 0.0f, offsetY = 0.0f;

	if (IsKeyDown(KEY_W)) {
		offsetY -= 1.0f;
	}
	if (IsKeyDown(KEY_S)) {
		offsetY += 1.0f;
	}
	if (IsKeyDown(KEY_A)) {
		offsetX -= 1.0f;
	}
	if (IsKeyDown(KEY_D)) {
		offsetX += 1.0f;
	}
	*/
	float speedIncrease = 0.0166f / 2.0f * (IsKeyDown(KEY_W) - IsKeyDown(KEY_S));
	player.speed *= 0.99f;
	bool onTrack = 0;
	for (int i = 0; i < track.straights.size(); i++) {
		if (i < track.straights.size()) {
			if (CheckCollisionRecs(track.straights[i], player.hitbox)) {
				onTrack = 1;
				break;
			}
		}
	}
	if (!onTrack) {
		for (int i = 0; i < track.corners.size(); i++) {
			if (CheckCollisionRecs(
				Rectangle{
					track.corners[i].position.x - track.corners[i].radius * !(track.corners[i].startAngle == 0 || track.corners[i].startAngle == 90),
					track.corners[i].position.y - track.corners[i].radius * !(track.corners[i].startAngle == 0 || track.corners[i].startAngle == 270),
					track.corners[i].radius,
					track.corners[i].radius },
					player.hitbox)) {
				onTrack = 1;
				break;
			}
		}
	}
	player.speed += speedIncrease + speedIncrease * onTrack;
	player.speed = Clamp(player.speed, -1.0f, 1.0f);

	player.rotation += 0.001f * (IsKeyDown(KEY_D) - IsKeyDown(KEY_A));
	player.rotation *= 0.975f;
	player.rotation = Clamp(player.rotation, -75.0f, 75.0f);

	player.direction = Vector2Rotate(player.direction, player.rotation);
	
	player.hitbox.x += player.direction.x * player.speed;
	player.hitbox.y += player.direction.y * player.speed;
	player.position = Vector2Add(player.position, Vector2Scale(player.direction, player.speed));
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void) {
	// TODO: Draw GAMEPLAY screen here!
	BeginMode2D(camera);

	ClearBackground(BLACK);

	Color bouncingColor = Color{ 245, 245, 245, 255 };
	// 170, 74, 68

	// Track
	for (int i = 0; i < track.corners.size(); i++) {
		drawCorner(&track.corners[i]);
	}
	for (int i = 0; i < track.straights.size(); i++) {
		DrawRectangle(track.straights[i].x,
			track.straights[i].y, 
			track.straights[i].width,
			track.straights[i].height,
			GRAY
		);
	}

	DrawTexture(player.apperance, player.hitbox.x, player.hitbox.y, WHITE);
	DrawLineV(player.position, Vector2Add(player.position, Vector2Scale(player.direction, player.speed * 20.0f)), GREEN);

	DrawRectangleLinesEx(background, 10.0f, WHITE);
	/*
	for (int i = 0; i < track.corners.size(); i++) {
		DrawRectangleLinesEx(Rectangle{
					track.corners[i].position.x - track.corners[i].radius * !(track.corners[i].startAngle == 0 || track.corners[i].startAngle == 90),
					track.corners[i].position.y - track.corners[i].radius * !(track.corners[i].startAngle == 0 || track.corners[i].startAngle == 270),
					track.corners[i].radius,
					track.corners[i].radius }, 10.0f, WHITE);
	}
	*/
	float healthFillup = player.hitbox.width * (player.fuel / 100.0f);
	DrawRectangle(player.hitbox.x, player.hitbox.y - 7.5f, healthFillup, 5.0f, RED);
	DrawRectangle(player.hitbox.x + healthFillup, player.hitbox.y - 7.5f, player.hitbox.width - healthFillup, 5.0f, BLACK);

	EndMode2D();

	/*
	DrawText(TextFormat("Boxy: %f", collisionBoxyCzas.count()), 400, 80, 20, RED);
	DrawText(TextFormat("Linie: %f", linieCzas.count()), 400, 100, 20, RED);
	DrawText(TextFormat("Hybryda: %f", hybridCzas.count()), 400, 120, 20, RED);
	*/
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {
	// TODO: Unload GAMEPLAY screen variables here!
	UnloadTexture(player.apperance);
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) {
	return finishScreen;
}

void drawCorner(Corner* corner) {
	DrawCircleSector(corner->position, corner->radius, corner->startAngle, corner->endAngle, corner->radius / 7.5f, GRAY);
	DrawCircleSector(corner->position, corner->radius - corner->width, corner->startAngle, corner->endAngle, corner->width / 7.5f, BLACK);
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