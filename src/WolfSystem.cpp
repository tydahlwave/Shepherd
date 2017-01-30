//
//  WolfSystem.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 1/29/17.
//
//

#include "WolfSystem.h"
#include "EntityFactory.h"
#include "Components/RigidBody.h"

void WolfSystem::Update(float deltaTime, World *world) {
    
    // Update timestep and spawn wolf if needed
    elapsedTime += deltaTime;
    if (elapsedTime > spawnRate) {
        elapsedTime = 0;
        if (wolves.size() <= maxEntities) {
            Spawn(world);
        }
    }
    
    // Move all the wolves
    for (GameObject *wolf : wolves) {
        MoveWolf(wolf, deltaTime, world);
    }
}

void WolfSystem::Spawn(World *world) {
    GameObject *wolf = EntityFactory::createWolf(world);
    glm::vec3 center = CalculateSheepCenter(world);
    center.y = 2;
    float angle = (float)(rand() % 360);
    glm::vec3 direction = glm::vec3(0, 0, 0);
    direction.x = sin(angle * 180 / M_PI);
    direction.z = sin(angle * 180 / M_PI);
    float distance = 20.0f;
    wolf->transform->position = center + direction * distance;
    wolves.push_back(wolf);
}

void WolfSystem::MoveWolf(GameObject *wolf, float deltaTime, World *world) {
    RigidBody *rigidBody = (RigidBody*) wolf->GetComponent("RigidBody");
    if (rigidBody) {
        GameObject *closestSheep = GetClosestSheep(wolf, world);
        if (closestSheep) {
            glm::vec3 direction = glm::normalize(closestSheep->transform->position - wolf->transform->position);
    //        glm::vec3 center = CalculateSheepCenter(world);
    //        glm::vec3 direction = glm::normalize(center - wolf->transform->position);
            rigidBody->velocity.x = direction.x * maxSpeed;
            rigidBody->velocity.z = direction.z * maxSpeed;
            float angle = atan2(direction.x, direction.z);
            wolf->transform->rotation.y = (angle * 180 / M_PI) + 90;
        } else {
            rigidBody->velocity.x = 0;
            rigidBody->velocity.z = 0;
        }
    }
}

glm::vec3 WolfSystem::CalculateSheepCenter(World *world) {
    glm::vec3 center = vec3(0, 0, 0);
    unsigned int sheepCount = 0;
    for (GameObject *go : world->GetGameObjects()) {
        // if the game object is a sheep
        if (go->name.compare("Bunny") == 0 && go->GetComponent("MeshRenderer")) {
            center += go->transform->position;
            sheepCount++;
        }
    }
    center /= sheepCount;
    return center;
}

GameObject *WolfSystem::GetClosestSheep(GameObject *wolf, World *world) {
    GameObject *closest = nullptr;
    float minDist = FLT_MAX;
    for (GameObject *go : world->GetGameObjects()) {
        // if the game object is a sheep
        if (go->name.compare("Bunny") == 0 && go->GetComponent("MeshRenderer")) {
            float dist = glm::distance(go->transform->position, wolf->transform->position);
            if (dist < minDist) {
                minDist = dist;
                closest = go;
            }
        }
    }
    return closest;
}
