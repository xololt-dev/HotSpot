#pragma once
#include "raylib.h"
// #include "gameObject.hpp"

class GameObject;

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
	void update(GameObject& gameObject) {
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
	}

private:
	float ACCELERATION = 1.0f;
};