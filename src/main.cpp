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
#include "TerrainController.h"
#include "Components/RigidBody.h"
#include "Components/TerrainRenderer.h"
#include "Terrain.h"
#include "BunnySpawnSystem.h"
#include "WolfSystem.h"
#include "TreeSystem.h"
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

#define SIMPLEX_TERRAIN 0
#define DIAMOND_SQUARE_TERRAIN 1

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

bool show_test_window = true;
bool show_another_window = true;
ImVec4 clear_color = ImColor(114, 144, 154);

void drawTerrainWindow(Window &window, GameObject *terrain) {
    TerrainRenderer *terrainRenderer = (TerrainRenderer*) terrain->GetComponent("TerrainRenderer");
    TextureLoader *textureTest = terrainRenderer->terrain->getTexture();

    // Prevent gui from being drawn in wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    ImGui_ImplGlfwGL3_NewFrame();
    
    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    {
        ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowContentSize(ImVec2(100, 20));
        ImGui::Begin("Debug");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    
    // 2. Show another simple window, this time using an explicit Begin/End pair
//    if (show_another_window)
//    {
//        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
//        ImGui::Begin("Another Window", &show_another_window);
//        ImGui::Text("Hello");
//        ImGui::End();
//    }
    
    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
//    if (show_test_window)
//    {
//        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
//        ImGui::ShowTestWindow(&show_test_window);
//    }
    
    {
        ImGui::SetNextWindowPos(ImVec2(20, 20), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Terrain Settings");
        ImVec2 uv0 = ImVec2(0, 0);
        ImVec2 uv1 = ImVec2(1, 1);
        ImGui::Image((void*)textureTest->getTextureId(), ImVec2(128, 128), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
        ImGui::End();
    }
    
    ImGui::Render();
    
    // Revert wireframe mode to how it was
    glPolygonMode(GL_FRONT_AND_BACK, window.drawWireframes ? GL_LINE : GL_FILL);
}

void setupWorld(World *world, TreeSystem &treeSystem) {
    // Create Cube (with bullet physics)
    EntityFactory::createSphere(world, 2.0, glm::vec3(5,20,2.0), 4.0);
    EntityFactory::createSphere(world, 2.0, glm::vec3(5,15,2.0), 4.0);
    EntityFactory::createSphere(world, 2.0, glm::vec3(5,10,2.0), 4.0);
    
    // Create Physics Ground (below previous ground)
    EntityFactory::createCube(world, glm::vec3(groundSize,0.1,groundSize), glm::vec3(5.5,-4,2.0),0);
    
    // Create boulders
    randomlyPopulateWithBoulders(world);
    
    // Create trees
    treeSystem.Spawn(world);
    
    EntityFactory::createHUD(world);
    
    //Create Path
    GameObject *path = EntityFactory::createPath(world, 4);
}

int main(int argc, char **argv) {
    handleInput(argc, argv);
    
    World world = World();
    Window window = Window(&world);
    Physics physics = Physics();
    Renderer renderer = Renderer();
    CameraController cameraController = CameraController();
    PhysicsController physicsController = PhysicsController();
    TerrainController terrainController = TerrainController();
    BunnySpawnSystem bunnySpawnSystem = BunnySpawnSystem();
    WolfSystem wolfSystem = WolfSystem();
    TreeSystem treeSystem = TreeSystem();
    
    // Static Initializers
    ModelLibrary::LoadModels(resourceDir);
    ShaderLibrary::LoadShaders(resourceDir);
    Texture::LoadTextures(resourceDir);
    MaterialLibrary::InitializeMaterials();
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&cameraController);
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&physicsController);
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&terrainController);
	Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)&bunnySpawnSystem);
    
    // Create terrain
    GameObject *terrain = EntityFactory::createTerrain(&world, resourceDir, SIMPLEX_TERRAIN, 1081, glm::vec3(0, -100, 0));
    terrain->transform->SetScale(glm::vec3(1, 1, 1));
    
    // Place game objects
    setupWorld(&world, treeSystem);

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
        // Reset current frame time
        oldTime = curTime;
        displayStats(elapsedTime, world, physics);
        
        accumulator += elapsedTime;
        
        while(accumulator >= idealDeltaTime) {
            //update
//            bunnySpawnSystem.Update(idealDeltaTime, &world, path);
//            wolfSystem.Update(idealDeltaTime, &world);
            physics.Update(idealDeltaTime, world);
            accumulator -= idealDeltaTime;
        }
        cameraController.Update(world);
        renderer.Render(world, window);
        
        if (window.drawGUI) drawTerrainWindow(window, terrain);
        window.Update();
    }
    
    return 0;
}
