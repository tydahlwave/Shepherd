//
//  CameraController.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#include "CameraController.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/RigidBody.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#define capVal(a, min, max) ((a = (a < min ? min : (a > max ? max : a))))
//#ifndef M_PI
//#define M_PI 3.14159265358979323846
//#endif

void CameraController::Update(World &world) {
	Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
	//get main character's position
	glm::vec3 pos = world.mainCamera->transform->GetPosition();
	//get main characters rotation
	glm::vec3 rot = world.mainCamera->transform->GetRotation();
	if (!camera->stat) {
		if (camera->dist > 0) {
			float offx = camera->dist * sin(glm::radians(camera->aap));
			float offy = camera->dist * sin(glm::radians(camera->pitch));
			float offz = camera->dist * cos(glm::radians(camera->aap));

			pos.x = pos.x - offx;
			pos.y = pos.y - offy;
			pos.z = pos.z - offz;
		}
		pos.y += 2.5 + camera->dist / 5.;
		if (camera->dist == 0) {
			((MeshRenderer*)world.mainCharacter->GetComponent("MeshRenderer"))->draw = false;
		}
		else {
			((MeshRenderer*)world.mainCharacter->GetComponent("MeshRenderer"))->draw = true;
		}
	}
	camera->yaw = glm::radians(camera->aap);
	camera->pos = pos;
	camera->lookAt = camera->pos + glm::vec3(sin(camera->yaw), sin(glm::radians(camera->pitch)), cos(camera->yaw));
}


void CameraController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
	Camera *camera = (Camera*)world->mainCamera->GetComponent("Camera");
	if (key == GLFW_KEY_UP && action == GLFW_PRESS) {
		camera->dist += .5;
		capVal(camera->dist, 0., 20);
	}
	else if (key == GLFW_KEY_DOWN && action == GLFW_PRESS) {
		camera->dist -= .5;
		capVal(camera->dist, 0., 20);
	}
}

void CameraController::MouseScrolled(World *world, double dx, double dy) {
	Camera *camera = (Camera*)world->mainCamera->GetComponent("Camera");
	camera->dist -= dy;
}

void CameraController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
		Camera * camera = (Camera*)world->mainCamera->GetComponent("Camera");
		if (camera->stat)
			return;
		glm::vec2 mouseCurr(mouseX, mouseY);
		if (mousePrev.x == 0 && mousePrev.y == 0) {
			mousePrev = mouseCurr;
		}
		glm::vec2 dv = mouseCurr - mousePrev;

		camera->aap -= dv.x;
		camera->pitch -= dv.y;

		capVal(camera->pitch, -60.f, 60.f);
		mousePrev = mouseCurr;
}

void CameraController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {
    
}
