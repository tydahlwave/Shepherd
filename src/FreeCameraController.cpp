//
//  FreeCameraController.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 3/3/17.
//
//

#include <iostream>
#include <GLFW/glfw3.h>

#include "World.h"
#include "FreeCameraController.h"
#include "Components/Camera.h"
#include "Components/RigidBody.h"

void FreeCameraController::Update(World &world) {
    
    // Get camera axes
    Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
    glm::vec3 gaze = camera->lookAt - world.mainCamera->transform->GetPosition();
    glm::vec3 w = normalize(-gaze);
    glm::vec3 u = normalize(cross(camera->up, w));
    
    // Move camera along velocity vector defined by keys being held down
    RigidBody *rigidBody = (RigidBody*)world.mainCamera->GetComponent("RigidBody");
    if (abs(rigidBody->velocity[2]) > STOP_THRESHOLD) {
        world.mainCamera->transform->SetPosition(world.mainCamera->transform->GetPosition() + rigidBody->velocity[2] * w);
        camera->lookAt += rigidBody->velocity[2] * w;
    }
    if (abs(rigidBody->velocity[0]) > STOP_THRESHOLD) {
        world.mainCamera->transform->SetPosition(world.mainCamera->transform->GetPosition() + rigidBody->velocity[0] * u);
        camera->lookAt += rigidBody->velocity[0] * u;
    }
}

void FreeCameraController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
    RigidBody *rigidBody = (RigidBody*)world->mainCamera->GetComponent("RigidBody");
    
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT_SHIFT) {
            cameraSpeed = FAST_CAMERA_SPEED;
        }
        if (key == GLFW_KEY_W) {
            rigidBody->velocity[2] = -cameraSpeed;
        } else if (key == GLFW_KEY_S) {
            rigidBody->velocity[2] = cameraSpeed;
        } else if (key == GLFW_KEY_A) {
            rigidBody->velocity[0] = -cameraSpeed;
        } else if (key == GLFW_KEY_D) {
            rigidBody->velocity[0] = cameraSpeed;
        }
    } else if (action == GLFW_RELEASE) {
        if (key == GLFW_KEY_LEFT_SHIFT) {
            cameraSpeed = SLOW_CAMERA_SPEED;
        }
        if (key == GLFW_KEY_W) {
            if (rigidBody->velocity[2] < -STOP_THRESHOLD) {
                rigidBody->velocity[2] = 0;
            } else {
                rigidBody->velocity[2] = cameraSpeed;
            }
        } else if (key == GLFW_KEY_S) {
            if (rigidBody->velocity[2] > STOP_THRESHOLD) {
                rigidBody->velocity[2] = 0;
            } else {
                rigidBody->velocity[2] = -cameraSpeed;
            }
        } else if (key == GLFW_KEY_A) {
            if (rigidBody->velocity[0] < -STOP_THRESHOLD) {
                rigidBody->velocity[0] = 0;
            } else {
                rigidBody->velocity[0] = cameraSpeed;
            }
        } else if (key == GLFW_KEY_D) {
            if (rigidBody->velocity[0] > STOP_THRESHOLD) {
                rigidBody->velocity[0] = 0;
            } else {
                rigidBody->velocity[0] = -cameraSpeed;
            }
        }
    }
    
    // Update current camera velocity to match the camera speed variable
    if (rigidBody->velocity.x > 0.01) {
        rigidBody->velocity.x = cameraSpeed;
    } else if (rigidBody->velocity.x < -0.01) {
        rigidBody->velocity.x = -cameraSpeed;
    }
    if (rigidBody->velocity.z > 0.01) {
        rigidBody->velocity.z = cameraSpeed;
    } else if (rigidBody->velocity.z < -0.01) {
        rigidBody->velocity.z = -cameraSpeed;
    }
}

void FreeCameraController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
    
    // Set the vertical view constraint to (-89,89) degrees
    double vertDegrees = M_PI*89/180;
    
    // Compute new alpha and beta for the camera lookat point
    double alpha2 = alpha + ((mouseY + windowHeight/2.0) / windowHeight * M_PI);
    double beta2 = beta + ((mouseX + windowWidth/2.0) / windowWidth * M_PI*2);
    
    // Constrain the view (up and down constrained to (-89,89) degrees)
    if (alpha2 > vertDegrees) alpha2 = vertDegrees;
    if (alpha2 < -vertDegrees) alpha2 = -vertDegrees;
    
    // Compute camera lookat point
    Camera *camera = (Camera*)world->mainCamera->GetComponent("Camera");
    camera->lookAt[0] = world->mainCamera->transform->GetPosition().x + cos(alpha2) * cos(beta2);
    camera->lookAt[1] = world->mainCamera->transform->GetPosition().y + -sin(alpha2);
    camera->lookAt[2] = world->mainCamera->transform->GetPosition().z + cos(alpha2) * cos(M_PI/2 - beta2);
}

void FreeCameraController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {}

void FreeCameraController::MouseScrolled(World *world, double dx, double dy) {

    // Get camera axes
    Camera *camera = (Camera*)world->mainCamera->GetComponent("Camera");
    glm::vec3 gaze = camera->lookAt - world->mainCamera->transform->GetPosition();
    glm::vec3 w = normalize(-gaze);
    glm::vec3 u = normalize(cross(camera->up, w));
    
    // Move camera along scroll direction
    if (abs(dy) > abs(dx)) {
        world->mainCamera->transform->SetPosition(world->mainCamera->transform->GetPosition() + (float)-dy * w);
        camera->lookAt += (float)-dy * w;
    } else {
        world->mainCamera->transform->SetPosition(world->mainCamera->transform->GetPosition() + (float)-dx * u);
        camera->lookAt += (float)-dx * u;
    }
}
