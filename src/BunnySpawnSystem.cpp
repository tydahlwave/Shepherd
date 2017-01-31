//
//  BunnySpawnSystem.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/24/17.
//
//

#include "BunnySpawnSystem.h"
#include "EntityFactory.h"
#include "Components/RigidBody.h"
#include <GLFW/glfw3.h>

void BunnySpawnSystem::Update(float deltaTime, World *world) {
	Flock(world);

    if (world->GetGameObjects().size() >= maxEntities) return;
    
    /*elapsedTime += deltaTime;
    if (elapsedTime > spawnRate) {
        elapsedTime = 0;*/
        GameObject *bunny = EntityFactory::createBunny(world);
        int randomAngle = rand() % 360;
        float velX = cos(randomAngle/180.0*M_PI);
        float velY = sin(randomAngle/180.0*M_PI);
        glm::vec3 vel = normalize(glm::vec3(velX, 0, velY)) * 5.0f;
        RigidBody *rigidBody = (RigidBody*)bunny->GetComponent("RigidBody");
        
        float floatX[maxEntities];
        float floatZ[maxEntities];
        vec3 randPosition;
        GameObject obj;
        bool positionClear = false;
        int randX;
        int randZ;
        
        
        //collect positions
        int i = 0;
        for(auto &obj : world->GetGameObjects()){
            randPosition = obj->transform->GetPosition();
            floatX[i] = randPosition.x;
            floatZ[i] = randPosition.z;
            i++;
        }
        
        
        //check if positions found
        while (!positionClear)
        {
            randX = rand() % 50 - 25;
            randZ = rand() % 50 - 25;
            
            bool xFound = false;
            bool zFound = false;
            
            //check through arrays
            for (int j = 0; j < maxEntities; j++) {
                if (floatX[j] == randX) {
                    xFound = true;
                }
                if (floatZ[j] == randZ) {
                    zFound = true;
                }
            }
            
            if (!(xFound && zFound)) {
                positionClear = true;
                randPosition = vec3(randX, 0, randZ);
				bunny->transform->SetPosition(randPosition);
            }
        }

		if (count == 0) {
			rigidBody->velocity -= vel;
		}
		count += 1;

        bunny->transform->SetPosition(randPosition);
        rigidBody->useGravity = true;
        bunny->transform->SetRotation(vec3(0, -randomAngle+90, 0));
        bunnies.push_back(bunny);
    //}
}

void BunnySpawnSystem::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
	if (action == GLFW_PRESS) {
		if (key == GLFW_KEY_F) {
			flockToCamera = !flockToCamera;
		}
	}
}

void BunnySpawnSystem::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {

}

void BunnySpawnSystem::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {

}


void BunnySpawnSystem::Flock(World *world) {
	Arrival(world, glm::vec3(0, 0, -25));
	Separation();
	Alignment();
	Cohesion();
}

void BunnySpawnSystem::Seek(World *world, glm::vec3 target) {
	for (GameObject *bunny : bunnies) {
		RigidBody *bunnyRigidBody = (RigidBody*)bunny->GetComponent("RigidBody");
        
        if (bunnyRigidBody) {

            float diffX = target.x - bunny->transform->GetPosition().x;
            float diffZ = target.z - bunny->transform->GetPosition().z;
            glm::vec3 desiredVel = glm::normalize(glm::vec3(diffX, 0, diffZ)) * maxSpeed;

            glm::vec3 steering = desiredVel - bunnyRigidBody->velocity;
            steering.y = 0;

            angle = atan2((target - bunny->transform->GetPosition()).x, (target - bunny->transform->GetPosition()).z) * 180.0 / 3.14;
            bunny->transform->SetRotation(glm::vec3(0, angle+90, 0));

            bunnyRigidBody->velocity += steering;
        }
	}
}

void BunnySpawnSystem::Arrival(World *world, glm::vec3 target) {
	float distX, distZ;
	glm::vec3 position = glm::vec3(0, 0, 0);

	for (GameObject *bunny : bunnies) {
		RigidBody *bunnyRigidBody = (RigidBody*)bunny->GetComponent("RigidBody");
        
        if (bunnyRigidBody) {
            
            if (flockToCamera) {
                position = world->mainCamera->transform->GetPosition();
            }
            else {
                position = target;
            }

            distX = position.x - bunny->transform->GetPosition().x;
            distZ = position.z - bunny->transform->GetPosition().z;
            angle = atan2((position - bunny->transform->GetPosition()).x, (position - bunny->transform->GetPosition()).z) * 180.0 / 3.14;
            bunny->transform->SetRotation(vec3(0, angle+90, 0));

            glm::vec3 desiredVel = glm::normalize(glm::vec3(distX, 0, distZ)) * maxSpeed;
            float dist = glm::length(desiredVel);
            desiredVel = glm::normalize(desiredVel);

            if (dist < 10.0f) {
                desiredVel *= maxSpeed * (dist / 10.0f);
            }
            else {
                desiredVel *= maxSpeed;
            }

            glm::vec3 steering = desiredVel - bunnyRigidBody->velocity;
            bunnyRigidBody->velocity += steering;
        }
	}
}

//Move away from objects we are too close to
void BunnySpawnSystem::Separation() {
	int neighborCount = 0;
	glm::vec3 steering = glm::vec3(0, 0, 0);
	std::vector<GameObject *>::iterator it;
	std::vector<GameObject *>::iterator it2;

	for (it = bunnies.begin(); it != bunnies.end(); ++it) {
		RigidBody *rigidBody = (RigidBody*)(*it)->GetComponent("RigidBody");
        if (rigidBody) {
            neighborCount = 0;
            steering = glm::vec3(0, 0, 0);
            for (it2 = bunnies.begin(); it2 != bunnies.end(); ++it2) {
                float distX = (*it)->transform->GetPosition().x - (*it2)->transform->GetPosition().x;
                float distZ = (*it)->transform->GetPosition().z - (*it2)->transform->GetPosition().z;
                if ((sqrt(pow(distX, 2) + pow(distZ, 2)) > 0) && (sqrt(pow(distX, 2) + pow(distZ, 2)) < 5.0)) {
                    float diffX = (*it)->transform->GetPosition().x - (*it2)->transform->GetPosition().x;
                    float diffZ = (*it)->transform->GetPosition().z - (*it2)->transform->GetPosition().z;
                    glm::vec3 diff = glm::normalize(glm::vec3(diffX, 0, diffZ));
                    steering.x += diff.x;
                    steering.z += diff.z;
                    neighborCount++;
                }
            }

            if (neighborCount > 0) {
                steering.x /= (float)neighborCount;
                steering.z /= (float)neighborCount;
            }

            rigidBody->velocity += steering * 6.0f;
        }
	}
}

//Change direction to line up/move closer with neighbors
void BunnySpawnSystem::Alignment() {
	int neighborCount = 0;
	glm::vec3 sum = glm::vec3(0, 0, 0);
	std::vector<GameObject *>::iterator it;
	std::vector<GameObject *>::iterator it2;

	for (it = bunnies.begin(); it != bunnies.end(); ++it) {
		RigidBody *rigidBody = (RigidBody*)(*it)->GetComponent("RigidBody");
        if (rigidBody) {
            neighborCount = 0;
            sum = glm::vec3(0, 0, 0);
            for (it2 = bunnies.begin(); it2 != bunnies.end(); ++it2) {
                RigidBody *rigidBody2 = (RigidBody*)(*it2)->GetComponent("RigidBody");
                if (rigidBody2) {
                    float distX = (*it)->transform->GetPosition().x - (*it2)->transform->GetPosition().x;
                    float distZ = (*it)->transform->GetPosition().z - (*it2)->transform->GetPosition().z;
                    if ((sqrt(pow(distX, 2) + pow(distZ, 2)) > 0) && (sqrt(pow(distX, 2) + pow(distZ, 2)) < 50)) {
                        sum.x += rigidBody2->velocity.x;
                        sum.z += rigidBody2->velocity.z;
                        neighborCount++;
                    }
                }
            }

            if (neighborCount > 0) {
                sum.x /= (float)neighborCount;
                sum.z /= (float)neighborCount;
            }

            rigidBody->velocity += sum;
        }
	}
}

//Move towards center of mass of all neighbors
void BunnySpawnSystem::Cohesion() {
	int neighborCount = 0;
	glm::vec3 sum = glm::vec3(0, 0, 0);
	std::vector<GameObject *>::iterator it;
	std::vector<GameObject *>::iterator it2;

	for (it = bunnies.begin(); it != bunnies.end(); ++it) {
		RigidBody *rigidBody = (RigidBody*)(*it)->GetComponent("RigidBody");
        if (rigidBody) {
            neighborCount = 0;
            sum = glm::vec3(0, 0, 0);
            for (it2 = bunnies.begin(); it2 != bunnies.end(); ++it2) {
                float distX = (*it)->transform->GetPosition().x - (*it2)->transform->GetPosition().x;
                float distZ = (*it)->transform->GetPosition().z - (*it2)->transform->GetPosition().z;
                if ((sqrt(pow(distX, 2) + pow(distZ, 2)) > 0) && (sqrt(pow(distX, 2) + pow(distZ, 2)) < 50)) {
                    sum.x += (*it2)->transform->GetPosition().x;
                    sum.z += (*it2)->transform->GetPosition().z;
                    neighborCount++;
                }
            }

            if (neighborCount > 0) {
                sum.x /= (float)neighborCount;
                sum.z /= (float)neighborCount;

                glm::vec3 steering = glm::normalize(sum - (*it)->transform->GetPosition());
                rigidBody->velocity += steering;
            }
            else {
                rigidBody->velocity += sum;
            }
        }
	}
}
