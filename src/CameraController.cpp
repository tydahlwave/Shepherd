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

void CameraController::Update(World &world) {
    Camera *camera = (Camera*)world.mainCharacter->GetComponent("Camera");
    RigidBody *rigidBody = (RigidBody*)world.mainCharacter->GetComponent("RigidBody");
    glm::vec3 gaze = camera->lookAt - world.mainCharacter->transform->GetPosition();
    glm::vec3 w = normalize(gaze);
    glm::vec3 u = normalize(cross(camera->up, w));
    if (abs(rigidBody->velocity[2]) > cameraStoppedThreshold) {
        //world.mainCharacter->transform->SetPosition(world.mainCharacter->transform->GetPosition() + rigidBody->velocity[2] * w);
        //camera->lookAt += rigidBody->velocity[2] * w;
    }
    if (abs(rigidBody->velocity[0]) > cameraStoppedThreshold) {
//        if ((world.mainCharacter->transform->GetPosition().y + rigidBody->velocity[0] * u.y) > -3.99) {
//            u.y = 0;
//            //world.mainCharacter->transform->position += rigidBody->velocity[0] * u;
//            //camera->lookAt += rigidBody->velocity[0] * u;
//        }
        //world.mainCharacter->transform->SetPosition(world.mainCharacter->transform->GetPosition() + rigidBody->velocity[0] * u);
        //camera->lookAt += rigidBody->velocity[0] * u;
    }
    
//    if (world.mainCharacter->transform->GetPosition().y < -1.99) {
//        world.mainCharacter->transform->SetPosition(glm::vec3(world.mainCharacter->transform->GetPosition().x,-1.99,world.mainCharacter->transform->GetPosition().z));
//    }
	camera->lookAt[0] = world.mainCharacter->transform->GetPosition().x + cos(alpha2) * cos(beta2);
	camera->lookAt[1] = world.mainCharacter->transform->GetPosition().y + -sin(alpha2);
	camera->lookAt[2] = world.mainCharacter->transform->GetPosition().z + cos(alpha2) * cos(M_PI / 2 - beta2);
	glm::vec3 dir = camera->lookAt - world.mainCharacter->transform->GetPosition();
	rigidBody->velocity = normalize(glm::vec3(dir.x, 0, dir.z)) * camera->vel[0] + normalize(glm::vec3(u.x, 0, u.z)) * camera->vel[2];
}


void CameraController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
	Camera *camera = (Camera*)world->mainCharacter->GetComponent("Camera");
    
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT_SHIFT) {
            cameraSpeed = 0.6;
        }
        if (key == GLFW_KEY_W) {
			camera->vel[0] = 5.f;
        } else if (key == GLFW_KEY_S) {
			camera->vel[0] = -5.f;
        } else if (key == GLFW_KEY_A) {
			camera->vel[2] = 5.f;
        } else if (key == GLFW_KEY_D) {
			camera->vel[2] = -5.f;
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT_SHIFT) {
            cameraSpeed = 0.2;
        }
        if (key == GLFW_KEY_W) {
			camera->vel[0] = 0.f;
        } else if (key == GLFW_KEY_S) {
			camera->vel[0] = 0.f;
        } else if (key == GLFW_KEY_A) {
			camera->vel[2] = 0.f;
        } else if (key == GLFW_KEY_D) {
			camera->vel[2] = 0.f;
        }
    }
    
}

void CameraController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
    // Compute new alpha and beta for the camera lookat point
    alpha2 = alpha + ((mouseY + windowHeight/2.0) / windowHeight * M_PI*16/18) - M_PI*8/18;
    beta2 = beta + ((mouseX + windowWidth/2.0) / windowWidth * M_PI*2) - M_PI;

    // Constrain the view (up and down constrained to (-80,80) degrees)
    if (alpha2 > M_PI*8/18) alpha2 = M_PI*8/18;
    if (alpha2 < -M_PI*8/18) alpha2 = -M_PI*8/18;

    // Compute camera lookat point
    Camera *camera = (Camera*)world->mainCharacter->GetComponent("Camera");
    camera->lookAt[0] = world->mainCharacter->transform->GetPosition().x + cos(alpha2) * cos(beta2);
    camera->lookAt[1] = world->mainCharacter->transform->GetPosition().y + -sin(alpha2);
    camera->lookAt[2] = world->mainCharacter->transform->GetPosition().z + cos(alpha2) * cos(M_PI/2 - beta2);
    //std::cout << camera->lookAt.x << ", " << camera->lookAt.y << ", " << camera->lookAt.z << std::endl;
}

void CameraController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {
    
}
