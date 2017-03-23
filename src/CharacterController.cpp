
#include "Components/ParticleRenderer.h"
#include "Components/TerrainRenderer.h"
#include "CharacterController.h"
#include "Components/Camera.h"
#include "Components/RigidBody.h"
#include "Components/Character.h"
#include "Components/Animation.h"
#include "EntityFactory.h"
#include "SoundLibrary.h"

#include <iostream>
#include <GLFW/glfw3.h>

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
    Animation* idleAnim = (Animation*) mainCharacter->GetComponent("Animation");
    idleAnim->anim = true;
    
    
    //idleAnim->skeleton.StopAnimating();
    //widleAnim->skeleton.SetIdleAnimation(&Anim_Test_Idle);
    //The true is for loop, and the false is for reset_to_start.
    

	if (action == GLFW_PRESS) {
        
		if (key == GLFW_KEY_LEFT_SHIFT) {
			moveSpeed = RUN_SPEED;
		}
		if (key == GLFW_KEY_W) {
			character->vel[2] = 5.f;
            wasd[0] = true;
            idleAnim->skeleton.PlayAnimation(Anim_Test_Idle,true,false);
            SoundLibrary::playWalk();
			if (dust == nullptr) {
				dust = EntityFactory::createParticleSystem(world, "Dust", 50, 2.0f, 0.5f, 0.5f, 0.1f, mainCharacter->transform->GetPosition());
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.3f));
				pr->particleSystem->setVelocityRange(glm::vec2(-0.5f, 0.5f), glm::vec2(0.0f, 0.3f), glm::vec2(-0.5f, 0.5f));
			}
			else {
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->setPosition(mainCharacter->transform->GetPosition());
				pr->particleSystem->systemLife = 0.5f;
			}
		}
		else if (key == GLFW_KEY_S) {
			character->vel[2] = -5.f;
            wasd[2] = true;
            idleAnim->skeleton.PlayAnimation(Anim_Test_Idle,true,false);
            SoundLibrary::playWalk();
			if (dust == nullptr) {
				dust = EntityFactory::createParticleSystem(world, "Dust", 50, 2.0f, 0.5f, 0.5f, 0.1f, mainCharacter->transform->GetPosition());
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.3f));
				pr->particleSystem->setVelocityRange(glm::vec2(-0.5f, 0.5f), glm::vec2(0.0f, 0.3f), glm::vec2(-0.5f, 0.5f));
			}
			else {
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->setPosition(mainCharacter->transform->GetPosition());
				pr->particleSystem->systemLife = 0.5f;
			}
		}
		else if (key == GLFW_KEY_A) {
			//	character->currentTurnSpeed = 30;
			character->vel[0] = 5.f;
            wasd[1] = true;
            idleAnim->skeleton.PlayAnimation(Anim_Test_Idle,true,false);
            SoundLibrary::playWalk();
			if (dust == nullptr) {
				dust = EntityFactory::createParticleSystem(world, "Dust", 50, 2.0f, 0.5f, 0.5f, 0.1f, mainCharacter->transform->GetPosition());
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.3f));
				pr->particleSystem->setVelocityRange(glm::vec2(-0.5f, 0.5f), glm::vec2(0.0f, 0.3f), glm::vec2(-0.5f, 0.5f));
			}
			else {
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->setPosition(mainCharacter->transform->GetPosition());
				pr->particleSystem->systemLife = 0.5f;
			}
		}
		else if (key == GLFW_KEY_D) {
			//	character->currentTurnSpeed = -30;
			character->vel[0] = -5.f;
            wasd[3] = true;
            idleAnim->skeleton.PlayAnimation(Anim_Test_Idle,true,false);
            SoundLibrary::playWalk();
			if (dust == nullptr) {
				dust = EntityFactory::createParticleSystem(world, "Dust", 50, 2.0f, 0.5f, 0.5f, 0.1f, mainCharacter->transform->GetPosition());
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.3f));
				pr->particleSystem->setVelocityRange(glm::vec2(-0.5f, 0.5f), glm::vec2(0.0f, 0.3f), glm::vec2(-0.5f, 0.5f));
			}
			else {
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->setPosition(mainCharacter->transform->GetPosition());
				pr->particleSystem->systemLife = 0.5f;
			}
		}
		else if (key == GLFW_KEY_SPACE) {
            
            GameObject*terrainObject;
            // Find the terrain
            for (GameObject *obj : world->GetGameObjects()) {
                if (obj->name.compare("Terrain") == 0) {
                    terrainObject = obj;
                    break;
                }
            }
            TerrainRenderer *terrainRenderer = (TerrainRenderer*)terrainObject->GetComponent("TerrainRenderer");
            Terrain *terrain = terrainRenderer->terrain;
            if (mainCharacter->transform->GetPosition().y + 15.0 < EntityFactory::getTerrainHeightForPosition(terrainObject, terrain, mainCharacter->transform->GetPosition().x, mainCharacter->transform->GetPosition().z)) {
                
                ((RigidBody*)mainCharacter->GetComponent("RigidBody"))->bulletRigidBody->setLinearVelocity(btVector3(0,60,0));
            }
			if (dust == nullptr) {
				dust = EntityFactory::createParticleSystem(world, "Dust", 30, 2.50f, 0.5f, 0.5f, 0.1f, mainCharacter->transform->GetPosition());
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->setColor(glm::vec4(0.0f, 0.0f, 0.0f, 0.3f));
				pr->particleSystem->setVelocityRange(glm::vec2(-0.5f, 0.5f), glm::vec2(0.0f, 0.3f), glm::vec2(-0.5f, 0.5f));
			}
			else {
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->setPosition(mainCharacter->transform->GetPosition());
				pr->particleSystem->systemLife = 0.5f;
			}
		}
	}
	else if (action == GLFW_RELEASE) {
        //idleAnim->skeleton.StopAnimating();
		if (key == GLFW_KEY_LEFT_SHIFT) {
			moveSpeed = MOVE_SPEED;
		}
		if (key == GLFW_KEY_W) {
			character->vel[2] = 0.f;
            wasd[0] = false;
            if(isWASD())
            {
                idleAnim->skeleton.StopAnimating();
                SoundLibrary::stopWalk();
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->systemLife = 0.3f;
            }
		}
		else if (key == GLFW_KEY_S) {
			character->vel[2] = 0.f;
            wasd[2] = false;
            if(isWASD())
            {
                idleAnim->skeleton.StopAnimating();
                SoundLibrary::stopWalk();
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->systemLife = 0.3f;
            }
		}
		else if (key == GLFW_KEY_A) {
			character->vel[0] = 0.f;
            wasd[1] = false;
            if(isWASD())
            {
                idleAnim->skeleton.StopAnimating();
                SoundLibrary::stopWalk();
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->systemLife = 0.3f;
            }		
		}
		else if (key == GLFW_KEY_D) {
			character->vel[0] = 0.f;
            wasd[3] = false;
            if(isWASD())
            {
                idleAnim->skeleton.StopAnimating();
                SoundLibrary::stopWalk();
				ParticleRenderer *pr = (ParticleRenderer*)dust->GetComponent("ParticleRenderer");
				pr->particleSystem->systemLife = 0.3f;
            }
		}
	}

}
bool CharacterController::isWASD() {
    return !wasd[0] && !wasd[1] && !wasd[2] && !wasd[3];
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
