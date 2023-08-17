#include "gameObject.hpp"

GameObject* createUser() {
	return new GameObject(
		new PhysicsObject(),
		new GraphicsObject(),
		new PlayerControlsObject()
	);
}