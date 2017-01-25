//
//  main.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#include <iostream>
#include <vector>

#include "Time.h"
#include "Window.h"
#include "World.h"
#include "GameObject.h"
#include "Mesh.h"
#include "Material.h"
#include "Shader.h"
#include "EntityFactory.h"
#include "Physics.h"
#include "Renderer.h"
#include "CameraController.h"
#include "RigidBody.h"
#include "BunnySpawnSystem.h"

static std::string resourceDir;

void handleInput(int argc, char **argv) {
    if (argc != 2) {
        std::cout << "Incorrect format" << std::endl;
        std::cout << "Expect: program [RESOURCE_DIR]" << std::endl;
        exit(0);
    }
    resourceDir = argv[1];
}

void displayStats(float deltaTime, World &world, Physics &physics) {
    static float elapsedTime = 0.5;
    static long frames = 0;
    elapsedTime += deltaTime;
    frames++;
    if (elapsedTime > 1.0) {
        elapsedTime = 0;
        int bunnyCount = 0;
        int groundedObjectsCount = 0;
        GameObject *ground;
        for (GameObject *gameObject : world.GetGameObjects()) {
            if (gameObject->name.compare("Ground")) {
                ground = gameObject;
                break;
            }
        }
        for (GameObject *gameObject : world.GetGameObjects()) {
            if (gameObject->name.compare("Bunny") == 0) {
                bunnyCount++;
                RigidBody *rigidBody = (RigidBody*)gameObject->GetComponent("RigidBody");
                if (abs(rigidBody->velocity.y) < 0.01) {
                    groundedObjectsCount++;
                }
            }
        }
        std::cout << "\nFPS: " << frames << std::endl;
        std::cout << "Game Objects: " << world.GetGameObjects().size() << std::endl;
        std::cout << "Bunnies: " << bunnyCount << std::endl;
        std::cout << "Objects on Ground: " << groundedObjectsCount << std::endl;
        std::cout << "Bunnies Collected: " << physics.bunniesCollected << std::endl;
        frames = 0;
    }
}

int main(int argc, char **argv) {
    handleInput(argc, argv);
    
    World world = World();
    Window window = Window(&world);
    Physics physics = Physics();
    Renderer renderer = Renderer();
    CameraController cameraController = CameraController();
    BunnySpawnSystem bunnySpawnSystem = BunnySpawnSystem();
    
    // Static Initializers
    Mesh::LoadMeshes(resourceDir);
    Shader::LoadShaders(resourceDir);
    Material::InitializeMaterials();
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&cameraController);
    
    // Create ground
    GameObject *ground = EntityFactory::createGround(&world);
    ground->transform->position.y -= 2;
    ground->transform->scale = glm::vec3(30, 1, 30);
    
    GameObject *barrier1 = EntityFactory::createBarrier(&world);
    barrier1->transform->position = glm::vec3(0, 0, -30);
    barrier1->transform->scale = glm::vec3(50, 5, 1);
    GameObject *barrier2 = EntityFactory::createBarrier(&world);
    barrier2->transform->position = glm::vec3(0, 0, 30);
    barrier2->transform->scale = glm::vec3(50, 5, 1);
    GameObject *barrier3 = EntityFactory::createBarrier(&world);
    barrier3->transform->position = glm::vec3(-30, 0, 0);
    barrier3->transform->scale = glm::vec3(1, 5, 50);
    GameObject *barrier4 = EntityFactory::createBarrier(&world);
    barrier4->transform->position = glm::vec3(30, 0, 0);
    barrier4->transform->scale = glm::vec3(1, 5, 50);
    
    // Seed random generator
    srand(time(0));
    
    // Init times
    long oldTime = Time::Now();
    
    std::cout << "Bunnies Collected: 0" << std::endl;
    
    // Game loop
    while (!window.ShouldClose()) {
        long curTime = Time::Now();
        float deltaTime = (curTime - oldTime) / 1000.0f;
        
        displayStats(deltaTime, world, physics);
        
        cameraController.Update(world);
        bunnySpawnSystem.Update(deltaTime, &world);
        physics.Update(deltaTime, world);
        renderer.Render(world, window);
        window.Update();
        
        // Reset current frame time
        oldTime = curTime;
    }
    
    return 0;
}
