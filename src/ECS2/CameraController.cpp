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
#include "Camera.h"
#include "RigidBody.h"

//#ifndef M_PI
//#define M_PI 3.14159265358979323846
//#endif

void CameraController::Update(World &world) {
    Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
    RigidBody *rigidBody = (RigidBody*)world.mainCamera->GetComponent("RigidBody");
    glm::vec3 gaze = camera->lookAt - world.mainCamera->transform->position;
    glm::vec3 w = normalize(-gaze);
    glm::vec3 u = normalize(cross(camera->up, w));
    if (abs(rigidBody->velocity[2]) > cameraStoppedThreshold) {
        world.mainCamera->transform->position += rigidBody->velocity[2] * w;
        camera->lookAt += rigidBody->velocity[2] * w;
    }
    if (abs(rigidBody->velocity[0]) > cameraStoppedThreshold) {
        if ((world.mainCamera->transform->position.y + rigidBody->velocity[0] * u.y) > -1.99) {
            u.y = 0;
            //world.mainCamera->transform->position += rigidBody->velocity[0] * u;
            //camera->lookAt += rigidBody->velocity[0] * u;
        }
        world.mainCamera->transform->position += rigidBody->velocity[0] * u;
        camera->lookAt += rigidBody->velocity[0] * u;
    }
    
    if (world.mainCamera->transform->position.y < -1.99) {
        world.mainCamera->transform->position.y = -1.99;
    }
}

void CameraController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
    RigidBody *rigidBody = (RigidBody*)world->mainCamera->GetComponent("RigidBody");
    
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_LEFT_SHIFT) {
            cameraSpeed = 0.6;
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
            cameraSpeed = 0.2;
        }
        if (key == GLFW_KEY_W) {
            if (rigidBody->velocity[2] < -cameraStoppedThreshold) {
                rigidBody->velocity[2] = 0;
            } else {
                rigidBody->velocity[2] = cameraSpeed;
            }
        } else if (key == GLFW_KEY_S) {
            if (rigidBody->velocity[2] > cameraStoppedThreshold) {
                rigidBody->velocity[2] = 0;
            } else {
                rigidBody->velocity[2] = -cameraSpeed;
            }
        } else if (key == GLFW_KEY_A) {
            if (rigidBody->velocity[0] < -cameraStoppedThreshold) {
                rigidBody->velocity[0] = 0;
            } else {
                rigidBody->velocity[0] = cameraSpeed;
            }
        } else if (key == GLFW_KEY_D) {
            if (rigidBody->velocity[0] > cameraStoppedThreshold) {
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

void CameraController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
    // Compute new alpha and beta for the camera lookat point
    double alpha2 = alpha + ((mouseY + windowHeight/2.0) / windowHeight * M_PI*16/18) - M_PI*8/18;
    double beta2 = beta + ((mouseX + windowWidth/2.0) / windowWidth * M_PI*2) - M_PI;

    // Constrain the view (up and down constrained to (-80,80) degrees)
    if (alpha2 > M_PI*8/18) alpha2 = M_PI*8/18;
    if (alpha2 < -M_PI*8/18) alpha2 = -M_PI*8/18;

    // Compute camera lookat point
    Camera *camera = (Camera*)world->mainCamera->GetComponent("Camera");
    camera->lookAt[0] = world->mainCamera->transform->position[0] + cos(alpha2) * cos(beta2);
    camera->lookAt[1] = world->mainCamera->transform->position[1] + -sin(alpha2);
    camera->lookAt[2] = world->mainCamera->transform->position[2] + cos(alpha2) * cos(M_PI/2 - beta2);
//    std::cout << camera->lookAt.x << ", " << camera->lookAt.y << ", " << camera->lookAt.z << std::endl;
}
