#pragma once
#include "raylib.h"
#include "physicsObject.hpp"
#include "graphicsObject.hpp"
#include "controlsObject.hpp"

class GameObject {
public:
	GameObject(PhysicsObject* physics_, GraphicsObject* graphics_, ControlsObject* controls_) :
		physics(physics_), graphics(graphics_), controls(controls_) {
		position = { 0.0f, 0.0f },
		velocity = { 0.0f, 0.0f };
	}

	GameObject(PhysicsObject* physics_, GraphicsObject* graphics_, ControlsObject* controls_, Vector2 position_, Vector2 velocity_) :
		physics(physics_), graphics(graphics_), controls(controls_), position(position_), velocity(velocity_)
	{}

	void update() {
		controls->update(*this);
		physics->update();
		graphics->update();
	}

private:
	Vector2 position;
	Vector2 velocity;

	// Controls
	ControlsObject* controls;
	// Physics
	PhysicsObject* physics;
	// Graphics
	GraphicsObject* graphics;	
};

GameObject* createUser();