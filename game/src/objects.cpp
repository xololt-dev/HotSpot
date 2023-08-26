#include "objects.hpp"
#include "raymath.h"
#include <iostream>

void WorldObject::initialize() {
	gameObjectsArray.push_back(createUser());

	PhysicsObject* tempPhysicsObject = new PhysicsObject(
		{ 100.0f, 100.0f,
		64.0f, 64.0f }
	);
	gameObjectsArray.push_back(
		new GameObject(
			tempPhysicsObject,
			new GraphicsObject(tempPhysicsObject),
			new StaticControlsObject(), 
			{ tempPhysicsObject->getHitbox().x, tempPhysicsObject->getHitbox().y },
			{ 0.0f, 0.0f },
			0.0f
		)
	);

	tempPhysicsObject = new PhysicsObject(
		{ 440.0f, 0.0f,
		10.0f, 450.0f }
	);
	gameObjectsArray.push_back(
		new GameObject(
			tempPhysicsObject,
			new GraphicsObject(tempPhysicsObject),
			new StaticControlsObject(),
			{ tempPhysicsObject->getHitbox().x, tempPhysicsObject->getHitbox().y },
			{ 0.0f, 0.0f },
			0.0f
		)
	);

	tempPhysicsObject = new PhysicsObject(
		{ 0.0f, 440.0f,
		450.0f, 10.0f }
	);
	gameObjectsArray.push_back(
		new GameObject(
			tempPhysicsObject,
			new GraphicsObject(tempPhysicsObject),
			new StaticControlsObject(),
			{ tempPhysicsObject->getHitbox().x, tempPhysicsObject->getHitbox().y },
			{ 0.0f, 0.0f },
			0.0f
		)
	);

	tempPhysicsObject = new PhysicsObject(
		{ 0.0f, 0.0f,
		10.0f, 450.0f }
	);
	gameObjectsArray.push_back(
		new GameObject(
			tempPhysicsObject,
			new GraphicsObject(tempPhysicsObject),
			new StaticControlsObject(),
			{ tempPhysicsObject->getHitbox().x, tempPhysicsObject->getHitbox().y },
			{ 0.0f, 0.0f },
			0.0f
		)
	);

	tempPhysicsObject = new PhysicsObject(
		{ 0.0f, 0.0f,
		450.0f, 10.0f }
	);
	gameObjectsArray.push_back(
		new GameObject(
			tempPhysicsObject,
			new GraphicsObject(tempPhysicsObject),
			new StaticControlsObject(),
			{ tempPhysicsObject->getHitbox().x, tempPhysicsObject->getHitbox().y },
			{ 0.0f, 0.0f },
			0.0f
		)
	);

	tempPhysicsObject = new PhysicsObject(
		{ 300.0f, 300.0f,
		16.0f, 16.0f }
	);
	gameObjectsArray.push_back(
		new GameObject(
			tempPhysicsObject,
			new GraphicsObject(tempPhysicsObject),
			new StaticControlsObject(1.0f),
			{ tempPhysicsObject->getHitbox().x, tempPhysicsObject->getHitbox().y },
			{ 2.0f, 0.0f },
			0.0f
		)
	);
}

void WorldObject::update() {
	std::vector<GameObject* >::iterator it;
	for (it = gameObjectsArray.begin(); it != gameObjectsArray.end(); it++) {
		(*it)->update(*this);
	}
}

void WorldObject::draw() {
	std::vector<GameObject* >::iterator it;
	for (it = gameObjectsArray.begin(); it != gameObjectsArray.end(); it++) {
		DrawRectanglePro(
			{	(*it)->getPhysicsObject()->getHitboxNew()->center.x,
				(*it)->getPhysicsObject()->getHitboxNew()->center.y,
				(*it)->getPhysicsObject()->getHitbox().width,
				(*it)->getPhysicsObject()->getHitbox().height },
			{ (*it)->getPhysicsObject()->getHitbox().width / 2.0f,
				(*it)->getPhysicsObject()->getHitbox().height / 2.0f },
			(*it)->rotation,
			RED
		);

		DrawRectangleLinesEx(
			(*it)->getPhysicsObject()->getHitbox(),
			5.0f,
			WHITE);
	}
}

void GameObject::update(WorldObject& world) {
	controls->update(*this);
	physics->update(*this, world);
	graphics->update(*this);
}

GameObject* createUser() {
	PhysicsObject* tempPhysicsObject = new PhysicsObject(
		{ 25.0f, 25.0f,
		32.0f, 32.0f }
	);

	return new GameObject(
		tempPhysicsObject,
		new GraphicsObject(tempPhysicsObject),
		new PlayerControlsObject()
	);
}

void PlayerControlsObject::update(GameObject& gameObject) {
	float rotate = (float)(IsKeyDown(KEY_E) - IsKeyDown(KEY_Q));
	float upDown = (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W)), leftRight = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));

#if DEBUG
	if (leftRight || upDown)
		std::cout << "Key value (C): " << leftRight << " " << upDown << "\n";
	if (rotate)
		std::cout << "Rotation direction: " << rotate << "\n";
#endif

	gameObject.velocity = Vector2Scale(Vector2Normalize({ leftRight, upDown }), ACCELERATION);
	gameObject.rotation = fmodf(gameObject.rotation + rotate, 360.0f);
}

void RectangularHitbox::rotate(float rotation) {
	Vector2 checkVec = { Vector2Add(center, Vector2Rotate({ -width / 2.0f, -height / 2.0f}, rotation * DEG2RAD)) };

	if ((checkVec.x != topLeft.x) || (checkVec.y != topLeft.y)) {
		topLeft = checkVec;
		topRight = { Vector2Add(center, Vector2Rotate({ width / 2.0f, -height / 2.0f}, rotation * DEG2RAD)) };
		bottomLeft = { Vector2Add(center, Vector2Rotate({ -width / 2.0f, height / 2.0f}, rotation * DEG2RAD)) };
		bottomRight = { Vector2Add(center, Vector2Rotate({ width / 2.0f, height / 2.0f}, rotation * DEG2RAD)) };
/*
#if DEBUG
		std::cout << "Top left: " << topLeft.x << " " << topLeft.y << " Top right: " << topRight.x << " " << topRight.y << "\n";
		std::cout << "Bottom left: " << bottomLeft.x << " " << bottomLeft.y << " Bottom right: " << bottomRight.x << " " << bottomRight.y << "\n";
#endif
*/
	}


}

void PhysicsObject::update(GameObject& gameObject, WorldObject& world) {
	Rectangle objectHitbox = { 0,0,0,0 };
	hitLocation closestHitLocation = NONE;
	std::vector<GameObject* >::iterator it;
	
	for (it = world.gameObjectsArray.begin(); it != world.gameObjectsArray.end(); it++) {
		// reference to self
		if (*it == &gameObject) continue;
		// object moving
		if (!(gameObject.velocity.x || gameObject.velocity.y)) continue;
		// object hit
		objectHitbox = (*it)->getPhysicsObject()->hitbox;
		if (!CheckCollisionRecs(this->hitbox, objectHitbox)) continue;

		// meat of the func
		Vector2 normal = { 0.0f, 0.0f };

		float rightXCurrent = hitbox.x + hitbox.width, bottomYCurrent = hitbox.y + hitbox.height;
		float rightXCompare = objectHitbox.x + objectHitbox.width, bottomYCompare = objectHitbox.y + objectHitbox.height;

		if (hitbox.x < objectHitbox.x) {
			if (rightXCurrent < rightXCompare) {
				// left top/bottom
				if (hitbox.y > objectHitbox.y) {
					// left &/ bottom
					if (hitbox.y + hitbox.height < objectHitbox.y + objectHitbox.height) {
						// left
#if DEBUG
						if(!gameObject.isStatic)
							std::cout << "left\n";
#endif
						closestHitLocation = LEFT;
						normal.x = -1.0f;
					}
					else if (hitbox.y + hitbox.height > objectHitbox.y + objectHitbox.height) {
						// bottom left
#if DEBUG
						if (!gameObject.isStatic)
							std::cout << "bottom left\n";
#endif
						closestHitLocation = BOTTOM_LEFT;
					}
				}
				else if (hitbox.y < objectHitbox.y) {
					// left &/ top
					if (hitbox.y + hitbox.height > objectHitbox.y + objectHitbox.height) {
						// left
#if DEBUG
						if (!gameObject.isStatic)
							std::cout << "left\n";
#endif
						closestHitLocation = LEFT;
						normal.x = -1.0f;
					}
					else if (hitbox.y + hitbox.height < objectHitbox.y + objectHitbox.height) {
						// top left
#if DEBUG
						if (!gameObject.isStatic)
							std::cout << "top left\n";
#endif
						closestHitLocation = TOP_LEFT;
					}
				}
			}
		}
		else if (hitbox.x > objectHitbox.x) {
			if (rightXCurrent < rightXCompare) {
				// top/bottom
				if (hitbox.y > objectHitbox.y) {
					// bottom
#if DEBUG
					if (!gameObject.isStatic)
						std::cout << "bottom\n";
#endif
					normal.y = 1.0f;
					closestHitLocation = BOTTOM;
				}
				else if (hitbox.y < objectHitbox.y) {
					// top
#if DEBUG
					if (!gameObject.isStatic)
						std::cout << "top\n";
#endif
					closestHitLocation = TOP;
					normal.y = -1.0f;
				}
			}
			else if (rightXCurrent > rightXCompare) {
				// right top/bottom
				if (hitbox.y > objectHitbox.y) {
					if (hitbox.y + hitbox.height < objectHitbox.y + objectHitbox.height) {
						// right
#if DEBUG
						if (!gameObject.isStatic)
							std::cout << "right\n";
#endif
						normal.x = 1.0f;
						closestHitLocation = RIGHT;
					}
					else if (hitbox.y + hitbox.height > objectHitbox.y + objectHitbox.height) {
						// bottom right
#if DEBUG
						if (!gameObject.isStatic)
							std::cout << "bottom right\n";
#endif
						closestHitLocation = BOTTOM_RIGHT;
					}
				}
				else if (hitbox.y < objectHitbox.y) {
					// right &/top
					if (hitbox.y + hitbox.height > objectHitbox.y + objectHitbox.height) {
						// right
#if DEBUG
						if (!gameObject.isStatic)
							std::cout << "right\n";
#endif
						closestHitLocation = RIGHT;
						normal.x = 1.0f;
					}
					else if (hitbox.y + hitbox.height < objectHitbox.y + objectHitbox.height) {
						// top right
#if DEBUG
						if (!gameObject.isStatic)
							std::cout << "top right\n";
#endif
						closestHitLocation = TOP_RIGHT;
					}
				}
			}
		}

#if DEBUG
		if ((gameObject.velocity.x || gameObject.velocity.y) && !gameObject.isStatic)
			std::cout << "Normal vector:" << normal.x << " " << normal.y << "\n";
#endif

		switch (closestHitLocation) {
		case LEFT:
		case RIGHT:
			gameObject.velocity.x = fabsf(gameObject.velocity.x) * normal.x;
			break;
		case TOP:
		case BOTTOM:
			gameObject.velocity.y = fabsf(gameObject.velocity.y) * normal.y;
			break;
		case TOP_LEFT:
		case BOTTOM_LEFT:
		case TOP_RIGHT:
		case BOTTOM_RIGHT:
			vectorReflection(gameObject.velocity, objectHitbox, closestHitLocation);
			break;
		case NONE:
		default:
			break;
		}

#if DEBUG
		if ((gameObject.velocity.x || gameObject.velocity.y) && !gameObject.isStatic)
			std::cout << "Velocity:" << gameObject.velocity.x << " " << gameObject.velocity.y << "\n";
#endif
	}
	// update stats
	gameObject.position = Vector2Add(gameObject.position, gameObject.velocity);
#if DEBUG
	if ((gameObject.velocity.x || gameObject.velocity.y) && !gameObject.isStatic)
		std::cout << "Position (P): " << gameObject.position.x << " " << gameObject.position.y << "\n";
#endif

	hitbox.x = gameObject.position.x;
	hitbox.y = gameObject.position.y;
#if DEBUG
	if (gameObject.rotation)
		std::cout << "Rotation: " << gameObject.rotation << "\n";
#endif
	hitboxNew->center = Vector2Add(hitboxNew->center, gameObject.velocity);
	// rotate here
	hitboxNew->rotate(gameObject.rotation);
}

void PhysicsObject::vectorReflection(Vector2& velocity, Rectangle& objectHitbox, hitLocation hitLocation_) {
	float distanceX = 0.0f, distanceY = 0.0f;

	switch (hitLocation_) {
	case TOP_LEFT:
		distanceX = objectHitbox.x + objectHitbox.width * 0.5f - (hitbox.x + hitbox.width * 0.5f);
		distanceY = objectHitbox.y + objectHitbox.height * 0.5f - (hitbox.y + hitbox.height * 0.5f);

		if (distanceX > distanceY) velocity.x = fabsf(velocity.x) * -1.0f; // normal.x = -1.0f;
		else velocity.y = fabsf(velocity.y) * -1.0f; // normal.y = -1.0f;
		break;
	case BOTTOM_LEFT:
		distanceX = objectHitbox.x + objectHitbox.width * 0.5f - (hitbox.x + hitbox.width * 0.5f);
		distanceY = hitbox.y + hitbox.height * 0.5f - (objectHitbox.y + objectHitbox.height * 0.5f);
		
		if (distanceX > distanceY) velocity.x = fabsf(velocity.x) * -1.0f; // normal.x = -1.0f;
		else velocity.y = fabsf(velocity.y); // normal.y = 1.0f;
		break;
	case TOP_RIGHT:
		distanceX = hitbox.x + hitbox.width * 0.5f - (objectHitbox.x + objectHitbox.width * 0.5f);
		distanceY = objectHitbox.y + objectHitbox.height * 0.5f - (hitbox.y + hitbox.height * 0.5f);

		if (distanceX > distanceY) velocity.x = fabsf(velocity.x); // normal.x = 1.0f;
		else velocity.y = fabsf(velocity.y) * -1.0f; // normal.y = -1.0f;
		break;
	case BOTTOM_RIGHT:
		distanceX = hitbox.x + hitbox.width * 0.5f - (objectHitbox.x + objectHitbox.width * 0.5f);
		distanceY = hitbox.y + hitbox.height * 0.5f - (objectHitbox.y + objectHitbox.height * 0.5f);

		if (distanceX > distanceY) velocity.x = fabsf(velocity.x); // normal.x = 1.0f;
		else velocity.y = fabsf(velocity.y); // normal.y = 1.0f;
		break;
	default:
#if DEBUG
		std::cout << "Why are you even here?????\n";
#endif
		break;
	}
}

bool PhysicsObject::checkCollision(RectangularHitbox& rectangle1, RectangularHitbox& rectangle2) {
	return 1;
}

/*
void GraphicsObject::update(GameObject& gameObject) {

}
*/