//
//  CameraController.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#include <iostream>
#include <GLFW/glfw3.h>

#include "CameraController.h"
#include "Components/Camera.h"
#include "Components/RigidBody.h"

//#ifndef M_PI
//#define M_PI 3.14159265358979323846
//#endif
//horizontal
//camera->dist * cos(camera->alpha2)
//vertical
//camera->dist * sin(camera->alpha2)
void CameraController::Update(World &world) {
    Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
	glm:vec3 pos = world.mainCamera->transform->GetPosition();
	//float theta = camera->aap - glm::radians(world.mainCamera->transform->GetRotation().y);
	//float theta = camera->aap;
	glm::vec3 rot = world.mainCamera->transform->GetRotation();
	std::cout << "aap " << camera->aap << " | Rot " << rot.x << ", " << rot.y << ", " << rot.z << std::endl;
	//float offx = camera->dist * cos(camera->pitch / 180 * M_PI) * sin(glm::radians(theta));
	float offx = camera->dist * sin(glm::radians(camera->aap));
	float offy = camera->dist * sin(glm::radians(camera->pitch));
	//float offz = camera->dist * cos(camera->pitch / 180 * M_PI) * cos(glm::radians(theta));
	float offz = camera->dist * cos(glm::radians(camera->aap));
	pos.x = pos.x - offx;
	pos.y = pos.y + 5;
	pos.z = pos.z - offz;
	camera->pos = pos;
	camera->yaw = glm::radians(camera->aap);
	


	//std::cout << "offs: " << offx << " | " << offy << " | " << offz << std::endl;
}


void CameraController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {

}

void CameraController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
	Camera *camera = (Camera*)world->mainCamera->GetComponent("Camera");

	glm::vec2 mouseCurr(mouseX, mouseY);
	if (mousePrev.x == 0 && mousePrev.y == 0)
		mousePrev = mouseCurr;
	glm::vec2 dv = mouseCurr - mousePrev;
	camera->aap -= dv.x;
	camera->pitch -= rfactor * dv.y;
	camera->pitch = camera->pitch > 60.f ? 60.f : camera->pitch;
	camera->pitch = camera->pitch < -60.f ? -60.f : camera->pitch;
	//camera->yaw = camera->yaw / 180 * M_PI;
	//camera->pitch = camera->pitch / 180 * M_PI;
	mousePrev = mouseCurr;
}

void CameraController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {
    
}
