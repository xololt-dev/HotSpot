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
#include <chrono>

#include <iostream>

//----------------------------------------------------------------------------------
// Module Variables Definition (local)
//----------------------------------------------------------------------------------
static int framesCounter = 0;
static int finishScreen = 0;

static int screenHeight = 0;
static int screenWidth = 0;

static float diagonal = 0.1f / sqrt(2);

struct Projectile {
	Vector2 position;
	Vector2 direction;
	float radius;
};

bool hasMoved = 0;
bool recalculateRays = 0;
int numberRays = 180;
int numberBounces = 4;
std::chrono::duration<double> collisionBoxyCzas = std::chrono::steady_clock::now() - std::chrono::steady_clock::now();
std::chrono::duration<double> linieCzas = collisionBoxyCzas;

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

void GenerateRayBounces();

// Gameplay Screen Initialization logic
void InitGameplayScreen(void)
{
	// TODO: Initialize GAMEPLAY screen variables here
	framesCounter = 0;
	finishScreen = 0;
	SetTargetFPS(60);

	screenHeight = GetRenderHeight();
	screenWidth = GetRenderWidth();
	
	camera = Camera2D{ {0.0f, 0.0f}, {0.0f, 0.0f}, 0.0f, 1.0f };
	player = Rectangle{ 400, 200, 20, 20 };
	dummy = Rectangle{ 600, 250, 20, 20 };
	background = Rectangle{ 0, 0, float(screenWidth), float(screenHeight) };
	pillar = Rectangle{ 100, 100, 50, 50 };

	if (rays.size() == 0) {
		for (int i = 0; i < numberRays; i++) {
			Vector2 temp = { 0.0f, -1.0f };
			temp = Vector2Rotate(temp, (360.0f / numberRays) * i * DEG2RAD);
			rays.push_back(Ray{ {player.x + 10.0f, player.y + 10.0f, 0.0f}, { temp.x, temp.y, 0.0f } });
		}
	}
	
	if (boundingBoxes.size() == 0) {
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
	
	GenerateRayBounces();

	RayCollision temp;
	float distance = 0.0f;
	collisionBoxyCzas = linieCzas = std::chrono::duration<double>::zero();
	auto end = std::chrono::steady_clock::now();
	auto start = std::chrono::steady_clock::now();
	std::chrono::duration<double> czas = end - start;
	
	for (int x = 0; x < 20000; x++) {
		start = start = std::chrono::steady_clock::now();
		for (int i = 0; i < rays.size(); i++) {
			for (int j = 0; j < boundingBoxes.size(); j++) {
				temp = GetRayCollisionBox(rays[i], boundingBoxes[j]);
				if (temp.hit && (distance > temp.distance)) {
					distance = temp.distance;
				}
			}
		}

		end = std::chrono::high_resolution_clock::now();

		czas = end - start;
		// std::cout << czas.count() << std::endl;
		collisionBoxyCzas += czas;

		start = std::chrono::high_resolution_clock::now();

		for (int i = 0; i < rays.size(); i++) {
			Vector2 startRay = { rays[i].position.x, rays[i].position.y };
			Vector2 endRay = { rays[i].direction.x, rays[i].direction.y };
			Vector2 collision = { 0.0f, 0.0f };
			endRay = Vector2Scale(endRay, 1000.0f);

			CheckCollisionLines(startRay, endRay, { dummy.x, dummy.y }, { dummy.x, dummy.y + dummy.height }, &collision);
			CheckCollisionLines(startRay, endRay, { dummy.x, dummy.y }, { dummy.x + dummy.width, dummy.y }, &collision);
			CheckCollisionLines(startRay, endRay, { dummy.x, dummy.y + dummy.height }, { dummy.x + dummy.width, dummy.y + dummy.height }, &collision);
			CheckCollisionLines(startRay, endRay, { dummy.x + dummy.width, dummy.y }, { dummy.x + dummy.width, dummy.y + dummy.height }, &collision);

			CheckCollisionLines(startRay, endRay, { pillar.x, pillar.y }, { pillar.x, pillar.y + pillar.height }, &collision);
			CheckCollisionLines(startRay, endRay, { pillar.x, pillar.y }, { pillar.x + pillar.width, pillar.y }, &collision);
			CheckCollisionLines(startRay, endRay, { pillar.x, pillar.y + pillar.height }, { pillar.x + pillar.width, pillar.y + pillar.height }, &collision);
			CheckCollisionLines(startRay, endRay, { pillar.x + pillar.width, pillar.y }, { pillar.x + pillar.width, pillar.y + pillar.height }, &collision);
			
			CheckCollisionLines(startRay, endRay, { 10.0f, 10.0f }, { 10.0f, 440.0f }, &collision);
			CheckCollisionLines(startRay, endRay, {10.0f, 10.0f}, {790.0f, 10.0f}, &collision);
			CheckCollisionLines(startRay, endRay, { 10.0f, 440.0f }, { 790.0f, 440.0f }, &collision);
			CheckCollisionLines(startRay, endRay, { 790.0f, 10.0f }, { 790.0f, 440.0f }, &collision);
		}

		end = std::chrono::high_resolution_clock::now();

		czas = end - start;
		linieCzas += czas;
	}
	// std::cout << linieCzas.count() << " " << collisionBoxyCzas.count() << std::endl;
}

// Gameplay Screen Update logic
void UpdateGameplayScreen(void)
{
	// TODO: Update GAMEPLAY screen variables here!
	hasMoved = 0;
	recalculateRays = 0;

	// Press enter or tap to change to ENDING screen
	if (IsKeyPressed(KEY_ENTER) || IsGestureDetected(GESTURE_TAP)) {
		finishScreen = 1;
		PlaySound(fxCoin);
		return;
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

	if (IsKeyPressed(KEY_EQUAL)) {
		if (IsKeyDown(KEY_LEFT_CONTROL)) {
			if (numberRays < 45) numberRays *= 3;
			else numberRays *= 2;
			recalculateRays = 1;
		}
		else {
			if (!numberBounces) numberBounces = 1;
			else numberBounces *= 2;
			hasMoved = 1;
		}
	}
	if (IsKeyPressed(KEY_MINUS)) {
		if (IsKeyDown(KEY_LEFT_CONTROL)) {
			if (numberRays > 45) numberRays /= 2;
			else if (numberRays > 5) numberRays /= 3;
			recalculateRays = 1;
		}
		else {
			numberBounces /= 2;
			hasMoved = 1;
		}		
	}

	if (recalculateRays) {
		rays.clear();
		hasMoved = 1;

		for (int i = 0; i < numberRays; i++) {
			Vector2 temp = { 0.0f, -1.0f };
			temp = Vector2Rotate(temp, (360.0f / numberRays) * i * DEG2RAD);
			rays.push_back(Ray{ {player.x + 10.0f, player.y + 10.0f, 0.0f}, { temp.x, temp.y, 0.0f } });
		}
	}

	if (hasMoved) {
		player.x += offsetX;
		player.y += offsetY;

		for (int i = 0; i < rays.size(); i++) {
			rays[i].position.x += offsetX;
			rays[i].position.y += offsetY;
		}

		GenerateRayBounces();
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
		ClearBackground(RAYWHITE);

		RayCollision temp, temp2;
		Color bouncingColor = Color{ 245, 245, 245, 255 };
		// 170, 74, 68
		bool dummyHit = false;
		unsigned short amountRays = rays.size();
		unsigned short amountBoxes = boundingBoxes.size();
		int amountBounces = bounces.size();
		
		for (int i = 0; i < amountRays; i++) {
			temp = GetRayCollisionBox(rays[i], boundingBoxes[0]);
			if (temp.hit) dummyHit = true;

			for (int j = 1; j < boundingBoxes.size(); j++) {
				temp2 = GetRayCollisionBox(rays[i], boundingBoxes[j]);
				if (temp2.hit) {
					if (!temp.hit || (temp2.distance < temp.distance)) temp = temp2;
				}
			}

			if (temp.hit) {
				DrawLineV({ rays[i].position.x, rays[i].position.y }, { temp.point.x, temp.point.y }, RAYWHITE);
			}
			else DrawRay(rays[i], RAYWHITE);
		}

		for (int i = 0; i < amountBounces; i++) {
			if (i % amountRays == 0) {
				bouncingColor.a *= 0.45f;
			};
			temp = GetRayCollisionBox(bounces[i], boundingBoxes[0]);
			if (temp.hit) dummyHit = true;

			for (int j = 1; j < amountBoxes; j++) {
				temp2 = GetRayCollisionBox(bounces[i], boundingBoxes[j]);
				// if (temp2.hit) {
				if (temp2.hit && temp2.distance > 0.001f) {
					if (!temp.hit || (temp2.distance < temp.distance))
						// if(temp2.distance > 0.001f) 
							temp = temp2;
				}
			}

			if (temp.hit) {
				DrawLine(bounces[i].position.x, bounces[i].position.y, temp.point.x, temp.point.y, bouncingColor);
			}
		}

		DrawRectangleRec(player, GOLD);
		if (dummyHit) DrawRectangleRec(dummy, BROWN);
		DrawRectangleLinesEx(background, 10.0f, BLACK);
		DrawRectangleRec(pillar, BLACK);
		
		for (int i = 0; i < projectiles.size(); i++) {
			DrawCircleV(projectiles[i].position, projectiles[i].radius, RED);                
		}

	EndMode2D();
	
	DrawText(TextFormat("Vector size: %04i", projectiles.size()), 400, 20, 20, RED);
	DrawText(TextFormat("Rays: %i", numberRays), 400, 40, 20, RED);
	DrawText(TextFormat("Bounces: %i", numberBounces), 400, 60, 20, RED);
	DrawText(TextFormat("Boxy: %f", collisionBoxyCzas.count()), 400, 80, 20, RED);
	DrawText(TextFormat("Linie: %f", linieCzas.count()), 400, 100, 20, RED);
}

// Gameplay Screen Unload logic
void UnloadGameplayScreen(void)
{
	// TODO: Unload GAMEPLAY screen variables here!
	rays.clear();
	bounces.clear();
}

// Gameplay Screen should finish?
int FinishGameplayScreen(void)
{
	return finishScreen;
}

// Generate ray bounces
void GenerateRayBounces() {
	bounces.clear();

	if (!numberBounces) return;

	int amountOfRays = rays.size();

	for (int x = 0; x < numberBounces; x++) {
		for (int i = x * amountOfRays; i < (x + 1) * amountOfRays; i++) {
			RayCollision finalCollisionForRay = RayCollision{ 0, 0.0f, {0.0f,0.0f,0.0f}, {0.0f,0.0f,0.0f} };
			int rayIndex = 0;

			for (int j = 0; j < boundingBoxes.size(); j++) {
				RayCollision temp;
				if (x > 0) temp = GetRayCollisionBox(bounces[i - amountOfRays], boundingBoxes[j]);
				else temp = GetRayCollisionBox(rays[i], boundingBoxes[j]);

				if (temp.distance > 0.001f) {
					if ((temp.hit && (temp.distance < finalCollisionForRay.distance)) || !finalCollisionForRay.hit) {
						finalCollisionForRay = temp;
						rayIndex = i;
						if (x) rayIndex -= amountOfRays;
					}
				}
			}

			if (finalCollisionForRay.hit) {
				Vector3 reflection = Vector3Reflect(
					x == 0 ? rays[rayIndex].direction : bounces[rayIndex].direction,
					finalCollisionForRay.normal);
				reflection.z = 0.0f;
				bounces.push_back(
					Ray{ finalCollisionForRay.point, reflection });
			}
		}
	}
}