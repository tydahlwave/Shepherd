//
//  PhysicsController.cpp
//
//  Created by Michael Cantrell on 1/23/17.
//
//

#include <iostream>
#include <GLFW/glfw3.h>

#include "PhysicsController.h"
#include "Components/Camera.h"
#include "Components/RigidBody.h"

void PhysicsController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {

}

void PhysicsController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
    
}

void PhysicsController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {
    if (key == GLFW_MOUSE_BUTTON_LEFT || key == GLFW_MOUSE_BUTTON_RIGHT) {
        float coef = -1.0;
        if(key == GLFW_MOUSE_BUTTON_LEFT) {
            coef = 1.0;
        }
        for(GameObject* go : world->GetGameObjects()) {
            if(go->name == "Sphere")
            {
                Camera* cam = (Camera*)world->mainCamera->GetComponent("Camera");
                RigidBody* rb = (RigidBody*)go->GetComponent("RigidBody");
                if(rb && rb->bulletRigidBody) {
                    btVector3 camPos = btVector3(world->mainCamera->transform->position.x,world->mainCamera->transform->position.y,world->mainCamera->transform->position.z);
                    btVector3 objPos = btVector3(go->transform->position.x, go->transform->position.y, go->transform->position.z);
                    btVector3 camLookAt = btVector3(cam->lookAt.x,cam->lookAt.y,cam->lookAt.z);
                    
                    btVector3 End = (camLookAt - camPos)*1000.0;
                    btCollisionWorld::ClosestRayResultCallback RayCallback(camPos, End);
                    world->dynamicsWorld->rayTest(camPos, End, RayCallback);
                    
                    if(RayCallback.m_collisionObject == rb->bulletRigidBody) {
                        rb->bulletRigidBody->activate();
                        rb->bulletRigidBody->applyForce(coef*200.0*(camLookAt - camPos), camPos - objPos);
                    }
                }
            }
        }
    }
}



