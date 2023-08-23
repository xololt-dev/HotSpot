#pragma once
#include "raylib.h"
#include <vector>
#include <iterator>

enum hitLocation {
	LEFT,
	RIGHT,
	TOP,
	BOTTOM
};

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
		std::vector<GameObject* >::iterator it;
		for (it = gameObjectsArray.begin(); it != gameObjectsArray.end(); it++) {
			delete *it;
		}
		gameObjectsArray.clear();
	}

	void initialize();
	void update();
	void draw();

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

class StaticControlsObject : public ControlsObject {
public:
	StaticControlsObject() {}

	StaticControlsObject(float ACCELERATION_) :
		ACCELERATION(ACCELERATION_)
	{}

	// enum for checking if controller/touchscreen/keyboard
	void update(GameObject& gameObject) {};

private:
	float ACCELERATION = 0.0f;
};

class PhysicsObject {
public:
	PhysicsObject(Rectangle hitbox_) : 
		hitbox(hitbox_) 
	{}

	void update(GameObject& gameObject, WorldObject& world);

	Rectangle getHitbox() { return hitbox; };

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

	void update(GameObject& gameObject) {};

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
		position = { physics->getHitbox().x, physics->getHitbox().y },
		velocity = { 0.0f, 0.0f };
	}

	GameObject(PhysicsObject* physics_, GraphicsObject* graphics_, ControlsObject* controls_, Vector2 position_, Vector2 velocity_) :
		physics(physics_), graphics(graphics_), controls(controls_), position(position_), velocity(velocity_)
	{}

	~GameObject() {
		delete controls;
		delete physics;
		delete graphics;
	}

	void update(WorldObject& world);

	PhysicsObject* getPhysicsObject() { return physics; }

private:
	// Controls
	ControlsObject* controls;
	// Physics - PhysicsProperties?
	PhysicsObject* physics;
	// Graphics
	GraphicsObject* graphics;
};

GameObject* createUser();