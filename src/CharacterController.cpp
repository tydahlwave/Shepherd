
#include <iostream>
#include <GLFW/glfw3.h>

#include "CharacterController.h"
#include "Components/Camera.h"
#include "Components/RigidBody.h"
#include "Components/Character.h"

void CharacterController::Update(World *world, float deltaTime) {
    GameObject *mainCharacter = (world->mainCharacter) ? world->mainCharacter : world->mainCamera;
	Character *character = (Character*)mainCharacter->GetComponent("Character");
	RigidBody *rigidBody = (RigidBody*)mainCharacter->GetComponent("RigidBody");
	//std::cout << pos.x << ", " << pos.y << ", " << pos.z << std::endl;
	glm::vec3 vel = character->vel;
	if (glm::length(vel) > 0) {
		glm::vec3 dir = normalize(glm::vec3(vel[0], 0, vel[2])) * moveSpeed;
		//std::cout << dir.x << ", " << dir.y << ", " << dir.z << std::endl;
		float theta = glm::radians(mainCharacter->transform->GetRotation().y);
		//float theta = 0;
		vel.x = dir.x*cos(theta) + dir.z*sin(theta);
		vel.y = dir.y;
		vel.z = dir.z*cos(theta) - dir.x*sin(theta);
	}
	else {
		vel.x = 0;
		vel.y = 0;
		vel.z = 0;
	}
	rigidBody->velocity = vel;
	Camera *camera = (Camera*)mainCharacter->GetComponent("Camera");
	mainCharacter->transform->SetRotation(glm::vec3(0, camera->aap, 0));
}

void CharacterController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
    GameObject *mainCharacter = (world->mainCharacter) ? world->mainCharacter : world->mainCamera;
	Character *character = (Character*)mainCharacter->GetComponent("Character");

	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_LEFT_SHIFT) {
			moveSpeed = 40;
		}
		if (key == GLFW_KEY_W) {
			character->vel[2] = 5.f;
		}
		else if (key == GLFW_KEY_S) {
			character->vel[2] = -5.f;
		}
		else if (key == GLFW_KEY_A) {
			//	character->currentTurnSpeed = 30;
			character->vel[0] = 5.f;
		}
		else if (key == GLFW_KEY_D) {
			//	character->currentTurnSpeed = -30;
			character->vel[0] = -5.f;
		}
		else if (key == GLFW_KEY_SPACE) {
			glm::vec3 pos = mainCharacter->transform->GetPosition();
			//pos.y += 100;
            ((RigidBody*)mainCharacter->GetComponent("RigidBody"))->bulletRigidBody->setLinearVelocity(btVector3(0,30,0));
			//mainCharacter->transform->SetPosition(pos);
		}
	}
	else if (action == GLFW_RELEASE) {
		if (key == GLFW_KEY_LEFT_SHIFT) {
			moveSpeed = 40;
		}
		if (key == GLFW_KEY_W) {
			character->vel[2] = 0.f;
		}
		else if (key == GLFW_KEY_S) {
			character->vel[2] = 0.f;
		}
		else if (key == GLFW_KEY_A) {
			character->vel[0] = 0.f;
		}
		else if (key == GLFW_KEY_D) {
			character->vel[0] = 0.f;
		}
	}

}

void CharacterController::MouseScrolled(World *world, double dx, double dy) {

}

void CharacterController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
    GameObject *mainCharacter = (world->mainCharacter) ? world->mainCharacter : world->mainCamera;
	glm::vec2 mouseCurr(mouseX, mouseY);
	if (mousePrev.x == 0 && mousePrev.y == 0)
		mousePrev = mouseCurr;
	glm::vec2 dv = mouseCurr - mousePrev;
	if (!dv.x)
		return;
	//std::cout << "Moving mouse! Angle change " << dv.x << std::endl;
	glm::vec3 rot = mainCharacter->transform->GetRotation();
	//std::cout << rot.y << std::endl;
	rot.y = rot.y - dv.x;
	//std::cout << rot.y << std::endl;
	mainCharacter->transform->SetRotation(rot);
	//std::cout << mainCharacter->transform->GetRotation().y << std::endl;

	mousePrev = mouseCurr;

}

void CharacterController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {

}
