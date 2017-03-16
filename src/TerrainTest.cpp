//
//  TerrainTest.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 3/3/17.
//
//

#include <iostream>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION

#include "Time.h"
#include "Mesh.h"
#include "TextureLibrary.h"
#include "Material.h"
#include "Shader.h"
#include "EntityFactory.h"
#include "Physics.h"
#include "Renderer.h"
#include "CharacterController.h"
#include "CameraController.h"
#include "FreeCameraController.h"
#include "PhysicsController.h"
#include "TerrainEditingController.h"
#include "Components/RigidBody.h"
#include "Components/TerrainRenderer.h"
#include "Terrain.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "BunnySpawnSystem.h"
#include "WolfSystem.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "Path.h"

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
    CameraController playerController = CameraController();
    CharacterController characterController = CharacterController();
    PhysicsController physicsController = PhysicsController();
    TerrainEditingController terrainController = TerrainEditingController();
    BunnySpawnSystem bunnySpawnSystem = BunnySpawnSystem();
    WolfSystem wolfSystem = WolfSystem();
    
    terrainController.SetResourceDir(resourceDir);
    bool playerControllersLinked = false;
    physics.enabled = false;
    
    // Static Initializers
    ModelLibrary::LoadModels(resourceDir);
    ShaderLibrary::LoadShaders(resourceDir);
    TextureLibrary::LoadTextures(resourceDir);
    MaterialLibrary::InitializeMaterials();
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&cameraController, 1);
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&terrainController, 1);
//    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&physicsController, 1);
//    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&playerController, 1);
//    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&characterController, 1);
//    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&bunnySpawnSystem, 1);
    Window::AddImguiUpdateDelegate((ImguiUpdateDelegate*)&terrainController);
    CAudioEngine::instance()->Init();
    CAudioEngine::instance()->LoadSounds(resourceDir);
    
    // Create terrain
    GameObject *terrain = EntityFactory::createTerrain(&world, resourceDir, SIMPLEX_TERRAIN, 256, glm::vec3(0, 0, 0));
    terrain->transform->SetScale(glm::vec3(3, 3, 3));
    terrainController.SetTerrain((TerrainRenderer*)terrain->GetComponent("TerrainRenderer"));
    
    // Initialize main camera position and lookvector
    world.mainCamera->transform->SetPosition(glm::vec3(0, 200, 200)*terrain->transform->GetScale());
    Camera *mainCamera = (Camera*)world.mainCamera->GetComponent("Camera");
    mainCamera->lookAt = glm::vec3(0, -1, -0.1); // Initial lookvector to orient the camera to look along -z axis (can't be (0,-1,0))
    
//    EntityFactory::createSphere(&world, 2, glm::vec3(0, 100, 0), 10);
    
    // Add directional light
    EntityFactory::createLight(&world, glm::vec3(1, 1, 1), true, glm::vec3(2, 2, 2), 1.0, 0.15, 1.0, glm::vec3(1, 1, 1));
    
    // Create skybox
    EntityFactory::createSkybox(&world, resourceDir);
    
    //Create Path
    std::vector<glm::vec3> pathPositions = {
        glm::vec3(50, -20, 50),
        glm::vec3(-40, -20, 70),
        glm::vec3(0, -20, 40)
    };
    GameObject *path = nullptr;
    
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
        
        if (world.mainCharacter && !playerControllersLinked) {
            playerControllersLinked = true;
            physics.enabled = true;
            Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&physicsController, 1);
            Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&playerController, 1);
            Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&characterController, 1);
            Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&bunnySpawnSystem, 1);
            path = EntityFactory::createPath(&world, terrain, pathPositions);
        }
        
        accumulator += elapsedTime;
        while(accumulator >= idealDeltaTime) {
            if (world.mainCharacter) {
                physics.Update(idealDeltaTime, world);
                bunnySpawnSystem.Update(idealDeltaTime, &world, path);
                wolfSystem.Update(idealDeltaTime, &world);
                characterController.Update(&world, idealDeltaTime);
            }
            accumulator -= idealDeltaTime;
        }
        
        if (world.mainCharacter) {
            glm::vec3 charPos = world.mainCharacter->transform->GetPosition();
            RigidBody *rigidBody = (RigidBody*)world.mainCharacter->GetComponent("RigidBody");
//            std::cout << "Character Position: (" << charPos.x << "," << charPos.y << "," << charPos.z << ")" << std::endl;
//            std::cout << "Character Velocity: (" << rigidBody->velocity.x << "," << rigidBody->velocity.y << "," << rigidBody->velocity.z << ")" << std::endl;
            playerController.Update(world);
        } else {
            cameraController.Update(world);
        }
        
        CAudioEngine::instance()->Update();
        renderer.Render(world, window);
        
        window.Update();
    }
    
    return 0;
}
