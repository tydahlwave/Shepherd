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
#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "EntityFactory.h"
#include "Physics.h"
#include "Renderer.h"
#include "CameraController.h"
#include "PhysicsController.h"
#include "Components/RigidBody.h"
#include "BunnySpawnSystem.h"
#include "WolfSystem.h"
#ifdef WIN32
#include <btBulletDynamicsCommon.h>
#else
#include <BulletDynamics/btBulletDynamicsCommon.h>
#endif
static std::string resourceDir;

static const float groundSize = 100.0f;

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
                RigidBody *rigidBody = (RigidBody*)gameObject->GetComponent("RigidBody");
                if (rigidBody) {
                    bunnyCount++;
                    if (abs(rigidBody->velocity.y) < 0.01) {
                        groundedObjectsCount++;
                    }
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

void randomlyPopulateWithBoulders(World *world) {
    for (int i = 0; i < 15; i++) {
        int type = rand() % 3;
        float scale = rand() % 4 + 1;
        GameObject *boulder = EntityFactory::createBoulder(world, type, 1);
        float posX = (rand() % (int)groundSize) - groundSize/2;
        float posZ = (rand() % (int)groundSize) - groundSize/2;
        boulder->transform->SetPosition(glm::vec3(posX, -4, posZ));
        boulder->transform->SetRotation(glm::vec3(0, rand() % 360, 0));
        boulder->transform->SetScale(glm::vec3(scale, scale, scale));
    }
}

int main(int argc, char **argv) {
    handleInput(argc, argv);
    
    World world = World();
    Window window = Window(&world);
    Physics physics = Physics();
    Renderer renderer = Renderer();
    CameraController cameraController = CameraController();
    PhysicsController physicsController = PhysicsController();
    BunnySpawnSystem bunnySpawnSystem = BunnySpawnSystem();
    WolfSystem wolfSystem = WolfSystem();
    
    // Static Initializers
    Mesh::LoadMeshes(resourceDir);
    Shader::LoadShaders(resourceDir);
    Texture::LoadTextures(resourceDir);
    Material::InitializeMaterials();
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&cameraController);
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&physicsController);
	Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&bunnySpawnSystem);
    
    // Create ground
//    GameObject *ground = EntityFactory::createGround(&world);
//    ground->transform->SetPosition(glm::vec3(ground->transform->GetPosition().x,ground->transform->GetPosition().y - 2,ground->transform->GetPosition().z));
//    ground->transform->SetScale(glm::vec3(30, 1, 30));
    
//    GameObject *barrier1 = EntityFactory::createBarrier(&world);
//    barrier1->transform->SetPosition(glm::vec3(0, 0, -30));
//    barrier1->transform->SetScale(glm::vec3(50, 5, 1));
//    GameObject *barrier2 = EntityFactory::createBarrier(&world);
//    barrier2->transform->SetPosition(glm::vec3(0, 0, 30));
//    barrier2->transform->SetScale(glm::vec3(50, 5, 1));
//    GameObject *barrier3 = EntityFactory::createBarrier(&world);
//    barrier3->transform->SetPosition(glm::vec3(-30, 0, 0));
//    barrier3->transform->SetScale(glm::vec3(1, 5, 50));
//    GameObject *barrier4 = EntityFactory::createBarrier(&world);
//    barrier4->transform->SetPosition(glm::vec3(30, 0, 0));
//    barrier4->transform->SetScale(glm::vec3(1, 5, 50));
    
//    GameObject *sphere = EntityFactory::createTexturedSphere(&world);
//    sphere->transform->SetPosition(glm::vec3(0, 0, -5));
    
    // Create Cube (with bullet physics)
//    GameObject *sphere1 = EntityFactory::createSphere(&world, 2.0, glm::vec3(5,20,2.0), 4.0);
//    GameObject *sphere2 = EntityFactory::createSphere(&world, 2.0, glm::vec3(5,15,2.0), 2.0);
//    GameObject *sphere3 = EntityFactory::createSphere(&world, 2.0, glm::vec3(5,10,2.0), 1.0);
    
    // Create Physics Ground (below previous ground)
    GameObject *cube2 = EntityFactory::createCube(&world, glm::vec3(groundSize,0.2,groundSize), glm::vec3(5.5,-4,2.0),0);
    
    // Create boulders
    randomlyPopulateWithBoulders(&world);
    
    EntityFactory::createHUD(&world);
    
    // Seed random generator
    srand(time(0));
    
    // Init times
    long oldTime = Time::Now();
    
    std::cout << "Bunnies Collected: 0" << std::endl;
    
    
    float idealDeltaTime = 1.f/60.f;
    float accumulator = 0.0f;
    
    // Game loop
    while (!window.ShouldClose()) {
        long curTime = Time::Now();
        float elapsedTime = (curTime - oldTime) / 1000.0f;
        // Reset current frame time
        oldTime = curTime;
        displayStats(elapsedTime, world, physics);
        
        accumulator += elapsedTime;
        
        while(accumulator >= idealDeltaTime) {
            //update
            bunnySpawnSystem.Update(idealDeltaTime, &world);
            wolfSystem.Update(idealDeltaTime, &world);
            physics.Update(idealDeltaTime, world);
            
            //endupdate
            accumulator -= idealDeltaTime;
        }
        cameraController.Update(world);
        renderer.Render(world, window);
        window.Update();
        
    }
    
    return 0;
}
