#pragma once
#ifndef CharacterController_h
#define CharacterController_h

#include <cmath>

#include "World.h"
#include "WindowCallbackDelegate.h"

class CharacterController : WindowCallbackDelegate {
public:
	CharacterController() {};
	virtual ~CharacterController() {};

	void Update(World *world, float deltaTime);
	void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action);
	void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY);
	void MouseClicked(World *world, double mouseX, double mouseY, int key, int action);
	void MouseScrolled(World *world, double dx, double dy);
private:
	float alpha = -M_PI / 2;
	float beta = -M_PI / 2 * 3;
	float turnSpeed = 0;
	float moveSpeed = 5;
	glm::vec2 mousePrev = { 0, 0 };
};

#endif /* CharacterController_h */
