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
// #include "rlImGui.h"

#include "screens.hpp"
#include "objects.hpp"

#include <algorithm>
#include <vector>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static int screenHeight = 0;
static int screenWidth = 0;

WorldObject world;
Camera2D camera;

Rectangle background;

//----------------------------------------------------------------------------------
// Gameplay Screen Functions Definition
//----------------------------------------------------------------------------------


// Gameplay Screen Initialization logic
void InitGameplayScreen(void) {
	// TODO: Initialize GAMEPLAY screen variables here
	framesCounter = 0;
	finishScreen = 0;

	screenHeight = GetRenderHeight();
	screenWidth = GetRenderWidth();
	
	camera = Camera2D { 
		{0.0f, 0.0f}, 
		{0.0f, 0.0f}, 
		0.0f, 
		1.0f 
	};

	world.initialize();

	//rlImGuiSetup(true);

	SetTargetFPS(60);
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void) {
	// TODO: Update GAMEPLAY screen variables here!
	if (!world.gameObjectsArray.size()) {
		world.initialize();
		// rlImGuiSetup(true);
	}

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
	
	world.update();
}

// Gameplay Screen Draw logic
void DrawGameplayScreen(void) {
	// TODO: Draw GAMEPLAY screen here!
	BeginMode2D(camera);
	ClearBackground(BLACK);

	world.draw();

	EndMode2D();
	/*
	// start ImGui Conent
	rlImGuiBegin();

	// show ImGui Content
	bool open = true;
	ImGui::ShowDemoWindow(&open);

	// end ImGui Content
	rlImGuiEnd();
	*/
	/*
	DrawText(TextFormat("Boxy: %f", collisionBoxyCzas.count()), 400, 80, 20, RED);
	DrawText(TextFormat("Linie: %f", linieCzas.count()), 400, 100, 20, RED);
	DrawText(TextFormat("Hybryda: %f", hybridCzas.count()), 400, 120, 20, RED);
	*/
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void) {
	// TODO: Unload GAMEPLAY screen variables here!
	world.~WorldObject();
	//rlImGuiShutdown();
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void) {
	return finishScreen;
}