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
#include "Components/TerrainRenderer.h"
#include "Time.h"
#include "Interpolation.h"

#include <iostream>
#include <GLFW/glfw3.h>
#include <glm/gtc/type_ptr.hpp>

#define capVal(a, min, max) ((a = (a < min ? min : (a > max ? max : a))))
//#ifndef M_PI
//#define M_PI 3.14159265358979323846
//#endif

float heightAtPoint(World &world, glm::vec3 pos) {
	std::vector<GameObject*> gameObjects = world.GetGameObjects();
	float interpolatedHeight = 0;
	GameObject *terrainObject = nullptr;

	// Find the terrain
	for (GameObject *obj : world.GetGameObjects()) {
		if (obj->name.compare("Terrain") == 0) {
			terrainObject = obj;
			break;
		}
	}
	if (!terrainObject) return interpolatedHeight;

	// Get terrain component
	TerrainRenderer *terrainRenderer = (TerrainRenderer*)terrainObject->GetComponent("TerrainRenderer");
	Terrain *terrain = terrainRenderer->terrain;
	glm::vec3 terrainSize = terrainObject->transform->GetScale() * glm::vec3(terrain->size, 1, terrain->size);
	glm::vec3 terrainPos = terrainObject->transform->GetPosition();
	glm::vec3 terrainMin = terrainObject->transform->GetPosition() - terrainSize / 2.0f;
	glm::vec3 terrainMax = terrainObject->transform->GetPosition() + terrainSize / 2.0f;
	if (pos.x >= terrainMin.x && pos.x < terrainMax.x - terrainObject->transform->GetScale().x &&
		pos.z >= terrainMin.z && pos.z < terrainMax.z - terrainObject->transform->GetScale().z) {
		float fColIndex = (pos.z - terrainMin.z) / terrainObject->transform->GetScale().z;
		float fRowIndex = (pos.x - terrainMin.x) / terrainObject->transform->GetScale().x;
		int rowIndex = (int)fRowIndex;// / (terrainSize.z/2.0f);
		int colIndex = (int)fColIndex;// / (terrainSize.x/2.0f);
		float neighbors[2][2] = {
			{ terrain->getHeight(rowIndex + 1, colIndex + 1), terrain->getHeight(rowIndex + 1, colIndex) },
			{ terrain->getHeight(rowIndex, colIndex + 1), terrain->getHeight(rowIndex, colIndex) }
		};

		//                std::cout << "Height[" << rowIndex << "][" << colIndex << "] = " << terrain->getHeight(rowIndex, colIndex) << std::endl;
		interpolatedHeight = BilinearInterpolate(neighbors, fColIndex - colIndex, fRowIndex - rowIndex);
		//                std::cout << "Interpolated Height: " << interpolatedHeight << std::endl;

		//                if(obj->name.compare("Wolf") != 0)
		//                obj->transform->SetPosition(glm::vec3(pos.x, terrainPos.y + interpolatedHeight * terrainObject->transform->GetScale().y + obj->transform->GetScale().y / 2.0f, pos.z));
	}
	//printf("Height is %f! at %f\n", interpolatedHeight, pos.y);
	return interpolatedHeight*terrainObject->transform->GetScale().y;
}

void CameraController::Update(World &world) {
	Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
	//get main character's position
	glm::vec3 pos = camera->pos;
	bool check = true;
	//get main characters rotation
//	glm::vec3 rot = world.mainCamera->transform->GetRotation();
	if (!camera->stat) {
		while (check) {
			pos = world.mainCamera->transform->GetPosition();
			pos.y += 2.5 + camera->dist / 5.;
			if (camera->dist > 0) {
				float offx = camera->dist * sin(glm::radians(camera->aap));
				float offy = camera->dist * sin(glm::radians(camera->pitch));
				float offz = camera->dist * cos(glm::radians(camera->aap));

				pos.x = pos.x - offx;
				pos.y = pos.y - offy;
				pos.z = pos.z - offz;
				if (pos.y < heightAtPoint(world, pos)) {
					camera->dist -= .5;
					capVal(camera->dist, 0., 20);
				}
				else {
					check = false;
				}
			}
			else {
				check = false;
			}
		}
		if (camera->dist < 1) {
			((MeshRenderer*)world.mainCharacter->GetComponent("MeshRenderer"))->draw = false;
		}
		else {
			((MeshRenderer*)world.mainCharacter->GetComponent("MeshRenderer"))->draw = true;
		}
	}
	camera->yaw = glm::radians(camera->aap);
	camera->pos = pos;
    float xAdjustment = 0;
    float yAdjustment = 0;
    if(shakesRemaining > 0)
    {
//        float shakeFreqX = 20;
//        float shakeFreqY = .5;
        float shakeFreqY2 = 25;
//        float shakeSizeX = .5;
//        float shakeSizeY = .5;
//        float shakeSizeY2 = 5;
        float t = Time::Now();
        xAdjustment = sin( t*shakeIntensity ) / 50;
        yAdjustment = sin( t*shakeIntensity ) / 75 + cos( t * shakeFreqY2 ) / 20;

        shakesRemaining--;
    }
    
	camera->lookAt = camera->pos + glm::vec3(sin(camera->yaw) + xAdjustment, sin(glm::radians(camera->pitch)) + yAdjustment, cos(camera->yaw));
}

void CameraController::BeginShaking(int framesToShake, float intensity)
{
    shakesRemaining = framesToShake;
    shakeIntensity = intensity;
}


void CameraController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
	Camera *camera = (Camera*)world->mainCamera->GetComponent("Camera");
	if (camera->stat && key == GLFW_KEY_P && action == GLFW_PRESS) {
		printf("pitch at %f | aap at %f\n", camera->pitch, camera->aap);
	}
	if (camera->stat)
		return;
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
	if (camera->stat)
		return;
	camera->dist -= dy;
	capVal(camera->dist, 0., 20);
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
