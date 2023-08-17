#pragma once
#include "raylib.h"

class PhysicsObject {
public:
	void update();

private:
	Rectangle hitbox;
	Vector2 acceleration;
	// float weight;
};