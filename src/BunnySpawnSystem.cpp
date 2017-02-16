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
#include "Components/PathRenderer.h"
#include <GLFW/glfw3.h>

void BunnySpawnSystem::Update(float deltaTime, World *world, GameObject *p) {
	//Flock(world, glm::vec3(0, 0, -25));

	PathRenderer *pathRenderer = (PathRenderer*)p->GetComponent("PathRenderer");
	path = pathRenderer->path;

	std::map<GameObject*, int>::iterator it;
	for (it = bunnyNode.begin(); it != bunnyNode.end(); ++it) {
		glm::vec3 target = FollowPath(world, it->first);
		Flock(world, it->first, target);
	}


//    if (bunnies.size() < maxEntities) {
    if (world->GetGameObjects().size() >= maxEntities) return;
    
    //elapsedTime += deltaTime;
    //if (elapsedTime > spawnRate) {
        elapsedTime = 0;
        GameObject *b = EntityFactory::createBunny(world);
        RigidBody *rigidBody = (RigidBody*)b->GetComponent("RigidBody");
        
        int randomAngle = rand() % 360;
        float velX = cos(randomAngle/180.0*M_PI);
        float velY = sin(randomAngle/180.0*M_PI);
        glm::vec3 vel = normalize(glm::vec3(velX, 0, velY)) * 5.0f;
        
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
			std::random_device rd;
			std::mt19937 mt(rd());
			std::uniform_real_distribution<float> distX(-15.0f, -25.0f);
			std::uniform_real_distribution<float> distZ(-15.0f, -25.0f);

            //randX = rand() % 50 - 25;
            //randZ = rand() % 50 - 25;
            
			randX = distX(mt);
			randZ = distZ(mt);
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
				b->transform->SetPosition(randPosition);
            }
        }

		if (count == 0) {
			rigidBody->velocity -= vel;
		}
		count += 1;
        
        b->transform->SetPosition(randPosition);
        rigidBody->useGravity = true;
        b->transform->SetRotation(vec3(0, -randomAngle, 0));
		bunnyNode.insert(std::make_pair(b, 0));
    //}
//    }
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

void BunnySpawnSystem::MouseScrolled(World *world, double dx, double dy) {

}

void BunnySpawnSystem::Flock(World *world, GameObject *bunny, glm::vec3 target) {
	Arrival(world, bunny, target);
	ObstacleAvoidance(world);
	Separation(bunny);
	//Alignment(bunny);
	Cohesion(bunny);
}

glm::vec3 BunnySpawnSystem::Seek(World *world, GameObject *bunny, glm::vec3 target) {
	std::map<GameObject*, int>::iterator it;
	glm::vec3 steering = glm::vec3(0, 0, 0);
	RigidBody *bunnyRigidBody = (RigidBody*)bunny->GetComponent("RigidBody");
        
    if (bunnyRigidBody) {
		float diffX = target.x - bunny->transform->GetPosition().x;
        float diffZ = target.z - bunny->transform->GetPosition().z;
        glm::vec3 desiredVel = glm::normalize(glm::vec3(diffX, 0, diffZ)) * maxSpeed;

        glm::vec3 steering = desiredVel - bunnyRigidBody->velocity;
        steering.y = 0;

        angle = atan2((target - bunny->transform->GetPosition()).x, (target - bunny->transform->GetPosition()).z) * 180.0 / 3.14;
        bunny->transform->SetRotation(glm::vec3(0, angle, 0));

		return steering;
    }
}

void BunnySpawnSystem::Arrival(World *world, GameObject *bunny, glm::vec3 target) {
	float distX, distZ;
	glm::vec3 position = glm::vec3(0, 0, 0);

	RigidBody *rigidBody = (RigidBody*)bunny->GetComponent("RigidBody");
		
	if (rigidBody) {
		if (flockToCamera) {
			position = world->mainCharacter->transform->GetPosition();
        }
        else {
			position = target;
        }

		distX = position.x - bunny->transform->GetPosition().x;
		distZ = position.z - bunny->transform->GetPosition().z;
        angle = atan2((position - bunny->transform->GetPosition()).x, (position - bunny->transform->GetPosition()).z) * 180.0 / 3.14;
		bunny->transform->SetRotation(vec3(0, angle, 0));

        glm::vec3 desiredVel = glm::normalize(glm::vec3(distX, 0, distZ)) * maxSpeed;
        float dist = glm::length(desiredVel);

        if (dist < 5.0f) {
            desiredVel *= maxSpeed * (dist / 5.0f);
        }
        else {
            desiredVel *= maxSpeed;
        }

        glm::vec3 steering = desiredVel - rigidBody->velocity;
        rigidBody->velocity += steering;
    }
}

//Move away from objects we are too close to
void BunnySpawnSystem::Separation(GameObject *bunny) {
	int neighborCount = 0;
	glm::vec3 steering = glm::vec3(0, 0, 0);
	std::map<GameObject*, int>::iterator it;
	std::map<GameObject*, int>::iterator it2;

	for (it = bunnyNode.begin(); it != bunnyNode.end(); ++it) {
		RigidBody *rigidBody = (RigidBody*)(it->first)->GetComponent("RigidBody");
        if (rigidBody) {
            neighborCount = 0;
            steering = glm::vec3(0, 0, 0);
            for (it2 = bunnyNode.begin(); it2 != bunnyNode.end(); ++it2) {
                float distX = (it->first)->transform->GetPosition().x - (it2->first)->transform->GetPosition().x;
                float distZ = (it->first)->transform->GetPosition().z - (it2->first)->transform->GetPosition().z;
                if ((sqrt(pow(distX, 2) + pow(distZ, 2)) > 0) && (sqrt(pow(distX, 2) + pow(distZ, 2)) < 3.0)) {
                    float diffX = (it->first)->transform->GetPosition().x - (it2->first)->transform->GetPosition().x;
                    float diffZ = (it->first)->transform->GetPosition().z - (it2->first)->transform->GetPosition().z;
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

            rigidBody->velocity += steering;
        }
	}
}

//Change direction to line up/move closer with neighbors
void BunnySpawnSystem::Alignment(GameObject *bunny) {
	int neighborCount = 0;
	glm::vec3 sum = glm::vec3(0, 0, 0);
	std::map<GameObject*, int>::iterator it;
	std::map<GameObject*, int>::iterator it2;

	for (it = bunnyNode.begin(); it != bunnyNode.end(); ++it) {
		RigidBody *rigidBody = (RigidBody*)(it->first)->GetComponent("RigidBody");
        if (rigidBody) {
            neighborCount = 0;
            sum = glm::vec3(0, 0, 0);
            for (it2 = bunnyNode.begin(); it2 != bunnyNode.end(); ++it2) {
                RigidBody *rigidBody2 = (RigidBody*)(it2->first)->GetComponent("RigidBody");
                if (rigidBody2) {
                    float distX = (it->first)->transform->GetPosition().x - (it2->first)->transform->GetPosition().x;
                    float distZ = (it->first)->transform->GetPosition().z - (it2->first)->transform->GetPosition().z;
                    if ((sqrt(pow(distX, 2) + pow(distZ, 2)) > 0) && (sqrt(pow(distX, 2) + pow(distZ, 2)) < 5)) {
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
void BunnySpawnSystem::Cohesion(GameObject *bunny) {
	int neighborCount = 0;
	glm::vec3 sum = glm::vec3(0, 0, 0);
	std::map<GameObject*, int>::iterator it;
	std::map<GameObject*, int>::iterator it2;

	for (it = bunnyNode.begin(); it != bunnyNode.end(); ++it) {
		RigidBody *rigidBody = (RigidBody*)(it->first)->GetComponent("RigidBody");
        if (rigidBody) {
            neighborCount = 0;
            sum = glm::vec3(0, 0, 0);
            for (it2 = bunnyNode.begin(); it2 != bunnyNode.end(); ++it2) {
                float distX = (it->first)->transform->GetPosition().x - (it2->first)->transform->GetPosition().x;
                float distZ = (it->first)->transform->GetPosition().z - (it2->first)->transform->GetPosition().z;
                if ((sqrt(pow(distX, 2) + pow(distZ, 2)) > 0) && (sqrt(pow(distX, 2) + pow(distZ, 2)) < 3)) {
                    sum.x += (it2->first)->transform->GetPosition().x;
                    sum.z += (it2->first)->transform->GetPosition().z;
                    neighborCount++;
                }
            }

            if (neighborCount > 0) {
                sum.x /= (float)neighborCount;
                sum.z /= (float)neighborCount;

                glm::vec3 steering = glm::normalize(sum - (it->first)->transform->GetPosition());
				rigidBody->velocity += steering;
            }
            else {
                rigidBody->velocity += sum;
            }
        }
	}
}

glm::vec3 BunnySpawnSystem::FollowPath(World *world, GameObject *bunny) {
	glm::vec3 target = glm::vec3 (0, 0, 0);

	if (path != NULL) {
		std::vector<glm::vec3> nodes = (*path).GetNodes();
		RigidBody *rigidBody = (RigidBody*)bunny->GetComponent("RigidBody");

		target = nodes.at(bunnyNode[bunny]);
        target.y = 0;
        glm::vec3 bunnyPos = bunny->transform->GetPosition();
        bunnyPos.y = 0;
		if (glm::distance(bunnyPos, target) <= (*path).radius) {
            // If at end node, don't continue
            if (glm::distance(nodes[path->size-1], target) > 0.1) {
                bunnyNode[bunny] = bunnyNode[bunny] + 1;

                if (bunnyNode[bunny] >= (*path).size) {
                    bunnyNode[bunny] = 0; //to loop around path
                    //bunnyNode[bunny] = (*path).size - 1;
                }
            }
		}
	}

	return target;
}

bool BunnySpawnSystem::LineIntersectsObj(glm::vec3 ahead, glm::vec3 ahead2, GameObject *obstacle) {
	return glm::distance(obstacle->transform->GetPosition(), ahead) <= 4.0 || glm::distance(obstacle->transform->GetPosition(), ahead2) <= 4.0;
}

GameObject *BunnySpawnSystem::FindClosestObstacle(World *world, GameObject *bunny, glm::vec3 ahead, glm::vec3 ahead2) {
	GameObject *closest = NULL;

	for (GameObject *obj : world->GetGameObjects()) {
		if ((obj->name.compare("Boulder") == 0 || obj->name.compare("Sphere") == 0) && obj->GetComponent("MeshRenderer")) {
			GameObject *obstacle = obj;
			bool collision = LineIntersectsObj(ahead, ahead2, obstacle);

			glm::vec3 position = bunny->transform->GetPosition();
			if (collision && (closest == NULL || glm::distance(position, obstacle->transform->GetPosition()) < glm::distance(position, closest->transform->GetPosition()))) {
				closest = obstacle;
			}
		}
	}

	return closest;
}

void BunnySpawnSystem::ObstacleAvoidance(World *world) {
	std::map<GameObject *, int>::iterator it;

	for (it = bunnyNode.begin(); it != bunnyNode.end(); ++it) {
		RigidBody *rigidBody = (RigidBody*)(it->first)->GetComponent("RigidBody");
		if (rigidBody) {
			float length = glm::length(rigidBody->velocity) / maxSpeed;
			glm::vec3 ahead = (it->first)->transform->GetPosition() + glm::normalize(rigidBody->velocity) * length;
			glm::vec3 ahead2 = (it->first)->transform->GetPosition() + glm::normalize(rigidBody->velocity) * length * 0.5f;

			GameObject *closest = FindClosestObstacle(world, it->first, ahead, ahead2);
			glm::vec3 steering = glm::vec3(0, 0, 0);

			if (closest != NULL) {
				steering.x = ahead.x - closest->transform->GetPosition().x;
				steering.z = ahead.z - closest->transform->GetPosition().z;

				steering = glm::normalize(steering) * 2.0f;
			}
			else {
				steering *= 0.0f;
			}

			rigidBody->velocity += steering;
		}
	}
}
