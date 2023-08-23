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
			{ 0.0f, 0.0f }
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
			{ 0.0f, 0.0f }
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
			{ 0.0f, 0.0f }
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
			{ 0.0f, 0.0f }
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
			{ 0.0f, 0.0f }
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
	// int keyValue = GetKeyPressed();
	// std::cout << "Key value: " << keyValue << "\n";
	float upDown = (float)(IsKeyDown(KEY_S) - IsKeyDown(KEY_W)), leftRight = (float)(IsKeyDown(KEY_D) - IsKeyDown(KEY_A));

	/*
	while (keyValue) {
		switch (keyValue) {
		case KEY_W:
			upDown += ACCELERATION;
			break;
		case KEY_S:
			upDown -= ACCELERATION;
			break;
		case KEY_A:
			leftRight -= ACCELERATION;
			break;
		case KEY_D:
			leftRight += ACCELERATION;
			break;
		}

		keyValue = GetKeyPressed();
		std::cout << "Key value: " << keyValue << "\n";
	}
	*/

	// gameObject.direction = { upDown, leftRight };
#if DEBUG
	if (leftRight || upDown)
		std::cout << "Key value (C): " << leftRight << " " << upDown << "\n";
#endif

	gameObject.velocity = Vector2Scale(Vector2Normalize({ leftRight, upDown }), ACCELERATION);
}

// still bug ?
// corner can send the object up (top right corner) because the suggested normal is different than desired
// maybe just make it check each side and call it a day at this point
/*
void PhysicsObject::update(GameObject& gameObject, WorldObject& world) {
#if DEBUG
	if (gameObject.velocity.x || gameObject.velocity.y)
		std::cout << "Key value (P): " << gameObject.velocity.x << " " << gameObject.velocity.y << "\n";
#endif

	// Check hitboxes
	std::vector<GameObject* >::iterator it;
	Rectangle objectHitbox = { 0,0,0,0 };
	for (it = world.gameObjectsArray.begin(); it != world.gameObjectsArray.end(); it++) {
		if (*it == &gameObject) continue;

		objectHitbox = (*it)->getPhysicsObject()->hitbox;
		if (!CheckCollisionRecs(this->hitbox, objectHitbox)) continue;
		// Calc here bounce direction and whenever change is needed (if objects are already going apart, no need to change the values again).

		if (gameObject.velocity.x || gameObject.velocity.y) {
#if DEBUG
			std::cout << "Hit\n";
#endif
		}
		else continue;

		Vector2 normal = 
			Vector2Subtract({ this->hitbox.x + this->hitbox.width * 0.5f, this->hitbox.y + this->hitbox.height * 0.5f },
				{ objectHitbox.x + objectHitbox.width * 0.5f, objectHitbox.y + objectHitbox.height * 0.5f });

#if DEBUG
		if (gameObject.velocity.x || gameObject.velocity.y)
			std::cout << "Normal:" << normal.x << " " << normal.y << "\n";
#endif

		float valueX = powf(normal.x, 2), valueY = powf(normal.y, 2);

		if (valueX < valueY)
			normal = { 0.0f, normal.y / sqrtf(valueY)};
		else if (valueX > valueY) 
			normal = { normal.x / sqrtf(valueX), 0.0f};

		normal = Vector2Normalize(normal);
#if DEBUG
		if (gameObject.velocity.x || gameObject.velocity.y)
			std::cout << "To add normal:" << normal.x << " " << normal.y << "\n";
#endif

		gameObject.velocity = Vector2Reflect(gameObject.velocity, normal);
		// gameObject.position = Vector2Add(gameObject.position, gameObject.velocity);
		
		// 0.1f offset
		if (normal.x > 0) {
			gameObject.position.x = objectHitbox.x + objectHitbox.width + 0.1f;
		}
		else if (normal.x < 0) {
			gameObject.position.x = objectHitbox.x - gameObject.getPhysicsObject()->hitbox.width - 0.1f;
		}

		if (normal.y > 0) {
			gameObject.position.y = objectHitbox.y + objectHitbox.height + 0.1f;
		}
		else if (normal.y < 0) {
			gameObject.position.y = objectHitbox.y - gameObject.getPhysicsObject()->hitbox.height - 0.1f;
		}
	}
	
	gameObject.position = Vector2Add(gameObject.position, gameObject.velocity);
#if DEBUG
	if (gameObject.velocity.x || gameObject.velocity.y)
		std::cout << "Position (P): " << gameObject.position.x << " " << gameObject.position.y << "\n";
#endif
	hitbox.x = gameObject.position.x;
	hitbox.y = gameObject.position.y;
}
*/
void PhysicsObject::update(GameObject& gameObject, WorldObject& world) {
	Rectangle objectHitbox = { 0,0,0,0 };
	float furthestDistance = -1.0f;
	hitLocation closestHitLocation;
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
						std::cout << "left\n";
						normal.x = -1.0f;
					}
					else if (hitbox.y + hitbox.height > objectHitbox.y + objectHitbox.height) {
						// bottom left
						std::cout << "bottom left\n";

						float distanceX = objectHitbox.x + objectHitbox.width * 0.5f - (hitbox.x + hitbox.width * 0.5f);
						float distanceY = hitbox.y + hitbox.height * 0.5f - (objectHitbox.y + objectHitbox.height * 0.5f);
						std::cout << distanceX << " " << distanceY << "\n";
						if (distanceX > distanceY) normal.x = -1.0f;
						else normal.y = 1.0f;
					}
				}
				else if (hitbox.y < objectHitbox.y) {
					// left &/ top
					if (hitbox.y + hitbox.height > objectHitbox.y + objectHitbox.height) {
						// left
						std::cout << "left\n";
						normal.x = -1.0f;
					}
					else if (hitbox.y + hitbox.height < objectHitbox.y + objectHitbox.height) {
						// top left
						std::cout << "top left\n";

						float distanceX = objectHitbox.x + objectHitbox.width * 0.5f - (hitbox.x + hitbox.width * 0.5f);
						float distanceY =  objectHitbox.y + objectHitbox.height * 0.5f - (hitbox.y + hitbox.height * 0.5f);
						std::cout << distanceX << " " << distanceY << "\n";
						if (distanceX > distanceY) normal.x = -1.0f;
						else normal.y = -1.0f;
					}
				}
			}
		}
		else if (hitbox.x > objectHitbox.x) {
			if (rightXCurrent < rightXCompare) {
				// top/bottom
				if (hitbox.y > objectHitbox.y) {
					// bottom
					std::cout << "bottom\n";
					normal.y = 1.0f;
				}
				else if (hitbox.y < objectHitbox.y) {
					// top
					std::cout << "top\n";
					normal.y = -1.0f;
				}
			}
			else if (rightXCurrent > rightXCompare) {
				// right top/bottom
				if (hitbox.y > objectHitbox.y) {
					if (hitbox.y + hitbox.height < objectHitbox.y + objectHitbox.height) {
						// right
						std::cout << "right\n";
						normal.x = 1.0f;
					}
					else if (hitbox.y + hitbox.height > objectHitbox.y + objectHitbox.height) {
						// bottom right
						std::cout << "bottom right\n";
						float distanceX = hitbox.x + hitbox.width * 0.5f - (objectHitbox.x + objectHitbox.width * 0.5f);
						float distanceY = hitbox.y + hitbox.height * 0.5f - (objectHitbox.y + objectHitbox.height * 0.5f);
						std::cout << distanceX << " " << distanceY << "\n";
						if (distanceX > distanceY) normal.x = -1.0f;
						else normal.y = 1.0f;
					}
				}
				else if (hitbox.y < objectHitbox.y) {
					// right &/top
					if (hitbox.y + hitbox.height > objectHitbox.y + objectHitbox.height) {
						// right
						std::cout << "right\n";
						normal.x = 1.0f;
					}
					else if (hitbox.y + hitbox.height < objectHitbox.y + objectHitbox.height) {
						// top right
						std::cout << "top right\n";
						float distanceX = hitbox.x + hitbox.width * 0.5f - (objectHitbox.x + objectHitbox.width * 0.5f);
						float distanceY = objectHitbox.y + objectHitbox.height * 0.5f - (hitbox.y + hitbox.height * 0.5f);
						std::cout << distanceX << " " << distanceY << "\n";
						if (distanceX > distanceY) normal.x = 1.0f;
						else normal.y = -1.0f;
					}
				}
			}
		}

#if DEBUG
		if (gameObject.velocity.x || gameObject.velocity.y)
			std::cout << "To add normal:" << normal.x << " " << normal.y << "\n";
#endif
		// prawdopodobnie trzeba oddzielne dla naro¿ników, bo inaczej wariacje s¹
		gameObject.velocity = Vector2Reflect(gameObject.velocity, normal);

#if DEBUG
		if (gameObject.velocity.x || gameObject.velocity.y)
			std::cout << "Velocity:" << gameObject.velocity.x << " " << gameObject.velocity.y << "\n";
#endif
	}
	// update stats
	gameObject.position = Vector2Add(gameObject.position, gameObject.velocity);
#if DEBUG
	if (gameObject.velocity.x || gameObject.velocity.y)
		std::cout << "Position (P): " << gameObject.position.x << " " << gameObject.position.y << "\n";
#endif

	hitbox.x = gameObject.position.x;
	hitbox.y = gameObject.position.y;
}


/*
void GraphicsObject::update(GameObject& gameObject) {

}
*/