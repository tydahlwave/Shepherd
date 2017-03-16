//
//  PhysicsController.cpp
//
//  Created by Michael Cantrell on 1/23/17.
//
//

#include <iostream>
#include <GLFW/glfw3.h>
#include <thread>

#include "PhysicsController.h"
#include "Components/Camera.h"
#include "Components/RigidBody.h"
#include "Time.h"
#include "SoundLibrary.h"
#include "Components/Death.h"
#include "CameraController.h"


bool PhysicsController::charge = false;
long PhysicsController::LeftClickPressTime = 0;
long PhysicsController::RightClickPressTime = 0;

void PhysicsController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {

}

void PhysicsController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
    
}

void PhysicsController::MouseScrolled(World *world, double dx, double dy) {

}
bool PhysicsController::isLyingInCone(btVector3 &x, btVector3 &t, btVector3 &b, float aperture){
    
    // This is for our convenience
    float halfAperture = aperture/2.f;
    
    // Vector pointing to X point from apex
    btVector3 apexToXVect = t-x;
    
    // Vector pointing from apex to circle-center point.
    btVector3 axisVect = t-b;
    
    // X is lying in cone only if it's lying in
    // infinite version of its cone -- that is,
    // not limited by "round basement".
    // We'll use dotProd() to
    // determine angle between apexToXVect and axis.
    bool isInInfiniteCone = apexToXVect.dot(axisVect) /apexToXVect.length() /axisVect.length()
    >
    // We can safely compare cos() of angles
    // between vectors instead of bare angles.
    cos(halfAperture);
    
    
    if(!isInInfiniteCone) return false;
    
    // X is contained in cone only if projection of apexToXVect to axis
    // is shorter than axis.
    // We'll use dotProd() to figure projection length.
    bool isUnderRoundCap = apexToXVect.dot(axisVect)/axisVect.length() < axisVect.length();
    return isUnderRoundCap;
}


void PhysicsController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {
    
    if (key == GLFW_MOUSE_BUTTON_LEFT || key == GLFW_MOUSE_BUTTON_RIGHT) {
        float coef = -1.0;
        if(key == GLFW_MOUSE_BUTTON_LEFT) {
            coef = 1.0;
        }
        if(action == GLFW_PRESS) {
            if(key == GLFW_MOUSE_BUTTON_LEFT){
                LeftClickPressTime = Time::Now();
            }
            else if(key == GLFW_MOUSE_BUTTON_RIGHT) {
                RightClickPressTime = Time::Now();
            }
            charge = true;
            SoundLibrary::playCharge();
            return;
        }
        if(action == GLFW_RELEASE){
            charge = false;
            //return; // might not need this.... just in case
            SoundLibrary::playShot();
            std::cout<<"delta time is:   "<< Time::Now() - LeftClickPressTime<<std::endl;
            float dT = (float)(Time::Now() - LeftClickPressTime);
            if (world->cameraController)
                 ((CameraController*)(world->cameraController))->BeginShaking((int)((dT/ 100) + .5), 200);
        }
        
        
        
        GameObject *mainCharacter = (world->mainCharacter) ? world->mainCharacter : world->mainCamera;
//        Camera* cam = (Camera*)mainCharacter->GetComponent("Camera");

		glm::vec3 rot = mainCharacter->transform->GetRotation();
		glm::vec3 pos = mainCharacter->transform->GetPosition();
		float theta = glm::radians(rot.y);
        btVector3 camPos = btVector3(pos.x, pos.y-2, pos.z);
        
        btVector3 camLookAt = btVector3(sin(theta) * 1, 0, cos(theta) * 1);
        btVector3 endPoint = camPos + (camLookAt)*30.0; // multiplied by a large number to make sure i got far enough
        
        
        btVector3 forceVector = camLookAt;
        forceVector.setY(coef*0.7);
        forceVector.normalize();
        
        // calculate force based on time
        long deltaTime;
        if(key == GLFW_MOUSE_BUTTON_LEFT) deltaTime = Time::Now() - LeftClickPressTime;
        else deltaTime = Time::Now() - RightClickPressTime;
        float forceScalar = deltaTime /30.;
        if(forceScalar > 200.) forceScalar = 200.;
        if(forceScalar < 40.) forceScalar = 30.;
        
        forceVector = coef*forceVector*forceScalar;
        
        
        for(GameObject* go : world->GetGameObjects()) {
            RigidBody* rb = (RigidBody*)go->GetComponent("RigidBody");
            if(rb && rb->bulletRigidBody)
            {
				if (!rb->isPushPullable)
					continue;
                
                btVector3 objPos = btVector3(go->transform->GetPosition().x, go->transform->GetPosition().y, go->transform->GetPosition().z);
                //btCollisionWorld::ClosestRayResultCallback RayCallback(camPos, endPoint);
                //world->dynamicsWorld->rayTest(camPos, endPoint, RayCallback);
                
                //if(RayCallback.m_collisionObject == rb->bulletRigidBody) {
                if(isLyingInCone(objPos, camPos, endPoint, M_PI/3.0)) {
                    //rb->bulletRigidBody->applyForce(forceVector, camPos - objPos);
                    //rb->bulletRigidBody->applyImpulse(forceVector, camPos - objPos);
                    //is sheep, baa
                    if(go->name.compare("Bunny") == 0)
                    {
                        SoundLibrary::playRandSheep();
//                        rb->bulletRigidBody->setLinearVelocity(forceVector);
                    }
                    else if(go->name.compare("Wolf") == 0)
                    {
                        SoundLibrary::playWolfHurt();
                        Death* gD = (Death*) go->GetComponent("Death");
                        if(gD){
//                            std::printf("found d comp");
                            if(forceScalar > 50){
//                                std::printf("set die");
                                gD->shouldDie = true;
                            }
                        }
//                        rb->bulletRigidBody->setLinearVelocity(forceVector);
                    }
                    else if(go->name.compare("Tree") == 0)
                    {
                        SoundLibrary::playTreeHit();
//                        glm::vec3 scale = go->transform->GetScale();
//                        scale.y *= 2;
//                        go->transform->SetScale(scale);
//                        rb->bulletRigidBody->setLinearVelocity(forceVector);
                    }
                    else if(go->name.compare("Rock") == 0)
                    {
                        SoundLibrary::playRockHit();
//                        rb->bulletRigidBody->setLinearVelocity(forceVector);
                    }
                    rb->bulletRigidBody->setLinearVelocity(forceVector);
                }
            }
        }
    }
}



