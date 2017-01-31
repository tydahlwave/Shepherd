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
#include "Time.h"

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
        if(action == GLFW_PRESS) {
            if(key == GLFW_MOUSE_BUTTON_LEFT) LeftClickPressTime = Time::Now();
            else if(key == GLFW_MOUSE_BUTTON_RIGHT) RightClickPressTime = Time::Now();
            return;
        }
        if(action != GLFW_RELEASE) return; // might not need this.... just in case
        for(GameObject* go : world->GetGameObjects()) {
            RigidBody* rb = (RigidBody*)go->GetComponent("RigidBody");
            if(rb && rb->bulletRigidBody)
            {
                Camera* cam = (Camera*)world->mainCamera->GetComponent("Camera");
                
                btVector3 camPos = btVector3(world->mainCamera->transform->GetPosition().x,world->mainCamera->transform->GetPosition().y,world->mainCamera->transform->GetPosition().z);
                btVector3 objPos = btVector3(go->transform->GetPosition().x, go->transform->GetPosition().y, go->transform->GetPosition().z);
                btVector3 camLookAt = btVector3(cam->lookAt.x,cam->lookAt.y,cam->lookAt.z);
                
                btVector3 End = (camLookAt - camPos)*1000.0;
                btCollisionWorld::ClosestRayResultCallback RayCallback(camPos, End);
                world->dynamicsWorld->rayTest(camPos, End, RayCallback);
                
                btVector3 forceVector = camLookAt - camPos;
                forceVector.setY(coef*0.7);
                forceVector.normalize();
                
                // calculate force based on time
                long deltaTime;
                if(key == GLFW_MOUSE_BUTTON_LEFT) deltaTime = Time::Now() - LeftClickPressTime;
                else deltaTime = Time::Now() - RightClickPressTime;
                float forceScalar = deltaTime /30.;
                if(forceScalar > 400.) forceScalar = 400.;
                
                forceVector = forceVector*forceScalar;
                
                if(RayCallback.m_collisionObject == rb->bulletRigidBody) {
                    //rb->bulletRigidBody->activate();
                    //rb->bulletRigidBody->applyForce(coef*forceVector, camPos - objPos);
                    //world->dynamicsWorld->updateSingleAabb(rb->bulletRigidBody);
                    rb->bulletRigidBody->setLinearVelocity(coef*forceVector);
                    //rb->velocity.y = 2000.;
                    
                }
            }
        }
    }
}



