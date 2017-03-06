//
//  TerrainTest.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 3/3/17.
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
#include "FreeCameraController.h"
#include "PhysicsController.h"
#include "TerrainEditingController.h"
#include "Components/RigidBody.h"
#include "Components/TerrainRenderer.h"
#include "Terrain.h"
#include "TextureLoader.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "Path.h"

#include "SOIL/Soil.h"

#ifdef WIN32
#include <btBulletDynamicsCommon.h>
#else
#include <BulletDynamics/btBulletDynamicsCommon.h>
#endif

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

int main(int argc, char **argv) {
    handleInput(argc, argv);
    
    World world = World();
    Window window = Window(&world, 1080, 920);
    Physics physics = Physics();
    Renderer renderer = Renderer();
    FreeCameraController cameraController = FreeCameraController();
    PhysicsController physicsController = PhysicsController();
    TerrainEditingController terrainController = TerrainEditingController();
    
    // Static Initializers
    ModelLibrary::LoadModels(resourceDir);
    ShaderLibrary::LoadShaders(resourceDir);
    Texture::LoadTextures(resourceDir);
    MaterialLibrary::InitializeMaterials();
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&cameraController);
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&physicsController);
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&terrainController);
    Window::AddImguiUpdateDelegate((ImguiUpdateDelegate*)&terrainController);
    CAudioEngine::instance()->Init();
    CAudioEngine::instance()->LoadSounds(resourceDir);
    
    // Initialize main camera position and lookvector
    world.mainCamera->transform->SetPosition(glm::vec3(0, 200, 200));
    Camera *mainCamera = (Camera*)world.mainCamera->GetComponent("Camera");
    mainCamera->lookAt = glm::vec3(0, -1, -0.1); // Initial lookvector to orient the camera to look along -z axis (can't be (0,-1,0))
    
    // Create terrain
    GameObject *terrain = EntityFactory::createTerrain(&world, resourceDir, SIMPLEX_TERRAIN, 256, glm::vec3(0, -100, 0));
    terrain->transform->SetScale(glm::vec3(1, 1, 1));
    terrainController.SetTerrain((TerrainRenderer*)terrain->GetComponent("TerrainRenderer"));
    
    EntityFactory::createSphere(&world, 2, glm::vec3(0, 100, 0), 10);
    
    // Seed random generator
    srand(time(0));
    
    // Init times
    long oldTime = Time::Now();
    float idealDeltaTime = 1.f/60.f;
    float accumulator = 0.0f;
    
    // Game loop
    while (!window.ShouldClose()) {
        long curTime = Time::Now();
        float elapsedTime = (curTime - oldTime) / 1000.0f;
        oldTime = curTime;
        
        displayStats(elapsedTime, world, physics);
        
        accumulator += elapsedTime;
        while(accumulator >= idealDeltaTime) {
//            physics.Update(idealDeltaTime, world);
            accumulator -= idealDeltaTime;
        }
        
        cameraController.Update(world);
        CAudioEngine::instance()->Update();
        renderer.Render(world, window);
        
        window.Update();
    }
    
    return 0;
}
