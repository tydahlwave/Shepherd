//
//  Physics.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#include <iostream>

#include "Physics.h"
#include "Components/RigidBody.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Components/MeshRenderer.h"

void Physics::Update(float deltaTime, World &world) {
    for (GameObject *gameObject : world.GetGameObjects()) {
        RigidBody *rigidBody = (RigidBody*)gameObject->GetComponent("RigidBody");
        if (rigidBody && rigidBody->useGravity && !rigidBody->isKinematic) {
            glm::vec3 accel = rigidBody->acceleration * deltaTime;
            glm::vec3 vel = rigidBody->velocity * deltaTime;
            rigidBody->acceleration += gravity * deltaTime;
            rigidBody->velocity += accel;
            gameObject->transform->position += vel;
        }
    }
    ComputeCollisions(world);
}

void Physics::ComputeCollisions(World &world) {
    std::vector<Collision> collisions;
    std::vector<GameObject*> gameObjects = world.GetGameObjects();
    // Only test for collision between 2 objects once
    for (int i = 0; i < gameObjects.size(); i++) {
        GameObject *gameObject1 = gameObjects[i];
    //for (GameObject *gameObject1 : world.GetGameObjects()) {
        // Get bounds for first object
        Bounds *bounds1 = BoundsForGameObject(gameObject1);
        // If the first object is not collidable, continue
        if (!bounds1) continue;
        for (int j = i+1; j < gameObjects.size(); j++) {
            GameObject *gameObject2 = gameObjects[j];
        //for (GameObject *gameObject2 : world.GetGameObjects()) {
            // Don't compute collision with self
            if (gameObject2 == gameObject1) continue;
            // Get bounds for second object
            Bounds *bounds2 = BoundsForGameObject(gameObject2);
            // If the second object is not collidable, continue
            if (!bounds2) continue;
            // Test for collision
            Bounds *intersection = bounds1->Intersection(bounds2);
            if (intersection) {
//                std::cout << gameObject1->name << " hit " << gameObject2->name << std::endl;
                Collision collision = Collision(gameObject1, gameObject2, intersection);
                collisions.push_back(collision);
            }
        }
    }
    ResolveCollisions(collisions);
}

void Physics::ResolveCollisions(std::vector<Collision> collisions) {
    for (Collision collision : collisions) {
        RigidBody *rigidBody1 = (RigidBody*)collision.gameObject1->GetComponent("RigidBody");
        RigidBody *rigidBody2 = (RigidBody*)collision.gameObject2->GetComponent("RigidBody");
        // Recompute bounds - they could have changed since last resolved collision
        Bounds *bounds1 = BoundsForGameObject(collision.gameObject1);
        Bounds *bounds2 = BoundsForGameObject(collision.gameObject2);
        // If there is no longer an intersection, continue
        Bounds *newIntersection = bounds1->Intersection(bounds2);
        if (!newIntersection) continue;
        
        if (collision.gameObject1->name.compare("MainCamera") == 0 && collision.gameObject2->name.compare("Bunny") == 0) {
            MeshRenderer *meshRenderer = (MeshRenderer*)collision.gameObject2->GetComponent("MeshRenderer");
            if (meshRenderer->material != Material::pearl) {
                meshRenderer->material = Material::pearl;
                rigidBody2->isKinematic = true;
                bunniesCollected += 1;
//                std::cout << "Bunnies Collected: " << bunniesCollected << std::endl;
            }
        } else if (collision.gameObject2->name.compare("MainCamera") == 0 && collision.gameObject1->name.compare("Bunny") == 0) {
            MeshRenderer *meshRenderer = (MeshRenderer*)collision.gameObject1->GetComponent("MeshRenderer");
            if (meshRenderer->material != Material::pearl) {
                meshRenderer->material = Material::pearl;
                rigidBody2->isKinematic = true;
                bunniesCollected += 1;
//                std::cout << "Bunnies Collected: " << bunniesCollected << std::endl;
            }
        } else if (collision.gameObject1->name.compare("Barrier") == 0 && collision.gameObject2->name.compare("Bunny") == 0) {
            rigidBody2->velocity.x *= -1;
            rigidBody2->velocity.z *= -1;
            collision.gameObject2->transform->position += rigidBody2->velocity / 5.0f;
            collision.gameObject2->transform->rotation.y -= 180;
        } else if (collision.gameObject2->name.compare("Barrier") == 0 && collision.gameObject1->name.compare("Bunny") == 0) {
            rigidBody1->velocity.x *= -1;
            rigidBody1->velocity.z *= -1;
            collision.gameObject1->transform->position += rigidBody1->velocity / 5.0f;
            collision.gameObject1->transform->rotation.y -= 180;
        } else {
        
        if (rigidBody1 && rigidBody2 && !rigidBody1->isKinematic && !rigidBody2->isKinematic) {
            // TODO: don't just move up
            float dy;
            if (bounds2->max.y > bounds1->max.y) {
                dy = bounds1->max.y - bounds2->min.y;
                collision.gameObject2->transform->position += glm::vec3(0, dy, 0);
                rigidBody1->velocity.y = rigidBody1->velocity.y/2;
                rigidBody1->acceleration = glm::vec3(0, 0, 0);
                rigidBody2->velocity.y = 0;
                rigidBody2->acceleration = glm::vec3(0, 0, 0);
            } else {
                dy = bounds2->max.y - bounds1->min.y;
                collision.gameObject1->transform->position += glm::vec3(0, dy, 0);
                rigidBody2->velocity.y = -rigidBody2->velocity.y/2;
                rigidBody2->acceleration = glm::vec3(0, 0, 0);
                rigidBody1->velocity.y = 0;
                rigidBody1->acceleration = glm::vec3(0, 0, 0);
            }
//            collision.gameObject1->transform->position += glm::vec3(0, dy/2, 0);
//            rigidBody1->velocity.y = 0;
//            rigidBody1->acceleration = glm::vec3(0, 0, 0);
//            rigidBody1->acceleration = rigidBody1->acceleration / glm::vec3(2, 2, 2);
//            rigidBody1->acceleration.y = -rigidBody1->acceleration.y;
//            collision.gameObject2->transform->position += glm::vec3(0, -dy/2, 0);
//            rigidBody2->velocity.y = 0;
//            rigidBody2->acceleration = glm::vec3(0, 0, 0);
//            rigidBody2->acceleration = rigidBody2->acceleration / glm::vec3(2, 2, 2);
//            rigidBody2->acceleration.y = -rigidBody2->acceleration.y;
        } else if (rigidBody1 && !rigidBody1->isKinematic) {
            // TODO: don't just move up
            float dy = bounds2->max.y - bounds1->min.y;
            collision.gameObject1->transform->position += glm::vec3(0, dy, 0);
            rigidBody1->velocity.y = 0;
            rigidBody1->acceleration = glm::vec3(0, 0, 0);
//            rigidBody1->acceleration = rigidBody1->acceleration / glm::vec3(2, 2, 2);
//            rigidBody1->acceleration.y = -rigidBody1->acceleration.y;
        } else if (rigidBody2 && !rigidBody2->isKinematic) {
            // TODO: don't just move up
            float dy = bounds1->max.y - bounds2->min.y;
            collision.gameObject2->transform->position += glm::vec3(0, dy, 0);
            rigidBody2->velocity.y = 0;
            rigidBody2->acceleration = glm::vec3(0, 0, 0);
//            rigidBody2->acceleration = rigidBody2->acceleration / glm::vec3(2, 2, 2);
//            rigidBody2->acceleration.y = -rigidBody2->acceleration.y;
        }
        }
    }
}

// Get the bounds for a game object if it is collidable, otherwise return null
Bounds *Physics::BoundsForGameObject(GameObject *gameObject) {
    MeshRenderer *meshRenderer = (MeshRenderer*)gameObject->GetComponent("MeshRenderer");
    BoxCollider *boxCollider = (BoxCollider*)gameObject->GetComponent("BoxCollider");
    SphereCollider *sphereCollider = (SphereCollider*)gameObject->GetComponent("SphereCollider");
    if (boxCollider || sphereCollider) {
        if (meshRenderer) {
            return meshRenderer->mesh->shape->bounds->TransformedBounds(gameObject->transform);
        }
        Bounds *bounds = new Bounds();
        return bounds->TransformedBounds(gameObject->transform);
    }
    return nullptr;
}