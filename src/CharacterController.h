#pragma once
#ifndef CharacterController_h
#define CharacterController_h

#include <cmath>

#include "World.h"
#include "WindowCallbackDelegate.h"
#include "BoneAnimation.h"

class CharacterController : WindowCallbackDelegate {
public:
	CharacterController() {};
	virtual ~CharacterController() {};
    
    std::vector<BoneAnimation> anims;
    BoneAnimation Anim_Test_Idle = *new BoneAnimation("idle", glm::vec2(0,2), 2);
    std::vector<bool> wasd = {false,false,false,false};

	void Update(World *world, float deltaTime);
	void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action);
	void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY);
	void MouseClicked(World *world, double mouseX, double mouseY, int key, int action);
	void MouseScrolled(World *world, double dx, double dy);
    bool isWASD();
private:
    const float MOVE_SPEED = 50;
    const float RUN_SPEED = 100;
//	float alpha = -M_PI / 2;
//	float beta = -M_PI / 2 * 3;
//	float turnSpeed = 0;
	float moveSpeed = MOVE_SPEED;
	glm::vec2 mousePrev = { 0, 0 };
	GameObject *dust = nullptr;
};

#endif /* CharacterController_h */
