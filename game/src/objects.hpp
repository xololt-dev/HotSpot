#pragma once
#include "raylib.h"
#include "raymath.h"
#include <vector>
#include <iterator>

enum hitLocation {
	NONE = 0,
	LEFT = 8,
	RIGHT = 1,
	TOP = 4,
	BOTTOM = 2,
	TOP_LEFT = 12,
	BOTTOM_LEFT = 10,
	TOP_RIGHT = 5,
	BOTTOM_RIGHT = 3
};

class GameObject;

class WorldObject {
public:
	// gameObjects
	std::vector<GameObject* > gameObjectsArray;

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
	virtual void update(GameObject& gameObject) {};
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

private:
	float ACCELERATION = 0.0f;
};

class Hitbox {
public:
	Vector2 center;
	Vector2 topLeft = { 0.0f, 0.0f };
	Vector2 topRight = { 0.0f, 0.0f };
	Vector2 bottomLeft = { 0.0f, 0.0f };
	Vector2 bottomRight = { 0.0f, 0.0f };

	Hitbox(Vector2 center_) :
		center(center_)
	{}

	// decide name etc
	virtual void checkHitbox() {};
	virtual void rotate(float rotation) = 0;
};

class RectangularHitbox : public Hitbox {
public:
	float height;
	float width;

	RectangularHitbox(Vector2 center_, float width_, float height_) :
		Hitbox(center_), width(width_), height(height_) {

		topLeft = { center.x - width / 2.0f, center.y - height / 2.0f };
		topRight = { center.x + width / 2.0f, center.y - height / 2.0f };
		bottomLeft = { center.x - width / 2.0f, center.y + height / 2.0f };
		bottomRight = { center.x + width / 2.0f, center.y + height / 2.0f };
	}

	RectangularHitbox(Vector2 center_, float width_, float height_, float rotation_) :
		Hitbox(center_), width(width_), height(height_) {
		// rotation from "top"

		topLeft = { Vector2Add(center, Vector2Rotate({ -width_ / 2.0f, -height_ / 2.0f}, rotation_)) };
		topRight = { Vector2Add(center, Vector2Rotate({ width_ / 2.0f, -height_ / 2.0f}, rotation_)) };
		bottomLeft = { Vector2Add(center, Vector2Rotate({ -width_ / 2.0f, height_ / 2.0f}, rotation_)) };
		bottomRight = { Vector2Add(center, Vector2Rotate({ width_ / 2.0f, height_ / 2.0f}, rotation_)) };
	}

	void rotate(float rotation);
};

class CircleHitbox : public Hitbox {
public:
	float radius;

	CircleHitbox(Vector2 center_, float radius_) :
		Hitbox(center_), radius(radius_)
	{}

	void checkHitbox() {}
};

/*
class EllipseHitbox : public CircleHitbox {
public:
	EllipseHitbox(Vector2 center_, float radius_, float a_, float b_) :
		CircleHitbox(center_, radius_), a(a_), b(b_)
	{}

	void checkHitbox() {}

	float a;
	float b;
};
*/

class PhysicsObject {
public:
	PhysicsObject(Rectangle hitbox_) : 
		hitbox(hitbox_) {
		hitboxNew = new RectangularHitbox(
			{ hitbox.x + hitbox.width / 2.0f, hitbox.y + hitbox.height / 2.0f },
			hitbox.width,
			hitbox.height
		);
	}

	PhysicsObject(Rectangle hitbox_, Hitbox* hitboxNew_) :
		hitbox(hitbox_), hitboxNew(hitboxNew_)
	{}

	~PhysicsObject() {
		delete hitboxNew;
	}

	void update(GameObject& gameObject, WorldObject& world);

	Rectangle getHitbox() { return hitbox; };
	Hitbox* getHitboxNew() { return hitboxNew; };

private:
	Rectangle hitbox;
	Hitbox* hitboxNew;

	void vectorReflection(Vector2& velocity, Rectangle& objectHitbox, hitLocation hitLocation_);

	// rec & rec, rec & circle, circle & circle
	// ellipse to be done later
	bool checkCollision(RectangularHitbox& rectangle1, RectangularHitbox& rectangle2);
	bool checkCollision(RectangularHitbox& rectangle, CircleHitbox& circle);
	bool checkCollision(CircleHitbox& circle1, CircleHitbox& circle2);
	
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
	float rotation = 0.0f;
	// if controls object == static
	bool isStatic;

	GameObject(PhysicsObject* physics_, GraphicsObject* graphics_, PlayerControlsObject* controls_) :
		physics(physics_), graphics(graphics_), controls(controls_) {
		position = { physics->getHitbox().x, physics->getHitbox().y },
		velocity = { 0.0f, 0.0f },
		isStatic = false;
	}

	GameObject(PhysicsObject* physics_, GraphicsObject* graphics_, StaticControlsObject* controls_) :
		physics(physics_), graphics(graphics_), controls(controls_) {
		position = { physics->getHitbox().x, physics->getHitbox().y },
		velocity = { 0.0f, 0.0f },
		isStatic = true;
	}

	GameObject(PhysicsObject* physics_, GraphicsObject* graphics_, PlayerControlsObject* controls_, Vector2 position_, Vector2 velocity_, float rotation_) :
		physics(physics_), graphics(graphics_), controls(controls_), position(position_), velocity(velocity_), rotation(rotation_) {
		isStatic = false;
	}

	GameObject(PhysicsObject* physics_, GraphicsObject* graphics_, StaticControlsObject* controls_, Vector2 position_, Vector2 velocity_, float rotation_) :
		physics(physics_), graphics(graphics_), controls(controls_), position(position_), velocity(velocity_), rotation(rotation_) {
		isStatic = true;
	}

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