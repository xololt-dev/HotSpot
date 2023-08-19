#include "objects.hpp"
#include "raymath.h"

void GameObject::update(WorldObject& world) {
	controls->update(*this);
	physics->update(*this, world);
	graphics->update(*this);
}

GameObject* createUser() {
	PhysicsObject* tempPhysicsObject = new PhysicsObject();

	return new GameObject(
		tempPhysicsObject,
		new GraphicsObject(tempPhysicsObject),
		new PlayerControlsObject()
	);
}

void PlayerControlsObject::update(GameObject& gameObject) {
	int keyValue = GetKeyPressed();
	float upDown = 0.0f, leftRight = 0.0f;

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
	}

	gameObject.direction = { upDown, leftRight };
}

void PhysicsObject::update(GameObject& gameObject, WorldObject& world) {
	// Check hitboxes
	std::vector<GameObject* >::iterator it;
	for (it = world.gameObjectsArray.begin(); it != world.gameObjectsArray.end(); it++) {
		if (*it == &gameObject) continue;
		if (!CheckCollisionRecs(this->hitbox, (*it)->getPhysicsObjects()->hitbox)) continue;
		// Calc here bounce direction and whenever change is needed (if objects are already going apart, no need to change the values again).
	}
}

void GraphicsObject::update(GameObject& gameObject) {

}