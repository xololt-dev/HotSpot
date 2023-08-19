#pragma once
#include "raylib.h"
#include <vector>
#include <iterator>

class GameObject;

class WorldObject {
public:
	// gameObjects
	std::vector<GameObject* > gameObjectsArray;
	/*
	std::vector<ControlsObject* > controlsObjectsArray;
	std::vector<PhysicsObject* > physicsObjectsArray;
	std::vector<GraphicsObject* > graphicsObjectsArray;
	*/
	~WorldObject() {
		gameObjectsArray.clear();
	}

	void initialize();

private:
	
};

class ControlsObject {
public:
	// virtual ~ControlsObject() {}

	// enum for checking if controller/touchscreen/keyboard
	virtual void update(GameObject& gameObject) = 0;
};

class PlayerControlsObject : public ControlsObject {
public:
	PlayerControlsObject() {}

	PlayerControlsObject(float ACCELERATION_) :
		ACCELERATION(ACCELERATION_)
	{}

	// enum for checking if controller/touchscreen/keyboard
	void update(GameObject& gameObject);

private:
	float ACCELERATION = 1.0f;
};

class PhysicsObject {
public:
	void update(GameObject& gameObject, WorldObject& world);

private:
	Rectangle hitbox;
	// Vector2 acceleration;
	// float weight;
};

class GraphicsObject {
public:
	GraphicsObject(PhysicsObject* physics_) :
		physics(physics_)
	{}

	void update(GameObject& gameObject);

private:
	// Physics
	PhysicsObject* physics;

	// Sprites here
	// Idle sprite
	// Move sprite
};

class GameObject {
public:
	Vector2 position;
	Vector2 velocity;
	Vector2 direction = { 0.0f, 0.0f };

	GameObject(PhysicsObject* physics_, GraphicsObject* graphics_, ControlsObject* controls_) :
		physics(physics_), graphics(graphics_), controls(controls_) {
		position = { 0.0f, 0.0f },
		velocity = { 0.0f, 0.0f };
	}

	GameObject(PhysicsObject* physics_, GraphicsObject* graphics_, ControlsObject* controls_, Vector2 position_, Vector2 velocity_) :
		physics(physics_), graphics(graphics_), controls(controls_), position(position_), velocity(velocity_)
	{}

	void update(WorldObject& world);

	PhysicsObject* getPhysicsObjects() { return physics; }

private:
	// Controls
	ControlsObject* controls;
	// Physics - PhysicsProperties?
	PhysicsObject* physics;
	// Graphics
	GraphicsObject* graphics;
};

GameObject* createUser();