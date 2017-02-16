
#include <iostream>
#include <ctime>

#include "Mesh.h"
#include "Texture.h"
#include "Material.h"
#include "Shader.h"
#include "EntityFactory.h"
#include "Physics.h"
#include "Renderer.h"
#include "CameraController.h"
#include "CharacterController.h"
#include "GameController.h"
#include "PhysicsController.h"
#include "TerrainController.h"
#include "Components/RigidBody.h"
#include "Components/TerrainRenderer.h"
#include "Terrain.h"
#include "BunnySpawnSystem.h"
#include "WolfSystem.h"
#include "AudioEngine.h"
#include "TreeSystem.h"
#include "TextureLoader.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "Time.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "Path.h"

#define SIMPLEX_TERRAIN 0
#define DIAMOND_SQUARE_TERRAIN 1


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


bool show_test_window = true;
bool show_another_window = true;
ImVec4 clear_color = ImColor(114, 144, 154);

void drawTerrainWindow(Window &window, GameObject *terrain) {
	TerrainRenderer *terrainRenderer = (TerrainRenderer*)terrain->GetComponent("TerrainRenderer");
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
		ImGui::Image((void*)textureTest->getTextureId(), ImVec2(128, 128), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		ImGui::End();
	}

	ImGui::Render();

	// Revert wireframe mode to how it was
	glPolygonMode(GL_FRONT_AND_BACK, window.drawWireframes ? GL_LINE : GL_FILL);
}


void GameController::Init(std::string resourceDir) {
	/*world = World();
	printf("world generated\n");
	window = Window(&world);
	printf("window generated\n");
	physics = Physics();
	printf("phys generated\n");
	renderer = Renderer();
	printf("rend generated\n");*/
	
	//initialize audio engine
	audio = CAudioEngine::instance();
	audio->Init();

	this->resourceDir = resourceDir;
	// Static Initializers
	ModelLibrary::LoadModels(resourceDir);
	ShaderLibrary::LoadShaders(resourceDir);
	Texture::LoadTextures(resourceDir);
	MaterialLibrary::InitializeMaterials();
}

void GameController::Run() {
	nextState = state = MainMenu;
	while (state != Close) {
		LoadState();
		// Seed random generator
		srand(time(0));

		// Init times
		long oldTime = Time::Now();
		float idealDeltaTime = 1.f / 60.f;
		float accumulator = 0.0f;

		audio->LoadSounds(resourceDir);

		//audio->PlaySound("herdAmbient.wav");

		// Game loop
		while (state == nextState) {
			long curTime = Time::Now();
			float elapsedTime = (curTime - oldTime) / 1000.0f;
			// Reset current frame time
			oldTime = curTime;
			displayStats(elapsedTime, world, physics);

			accumulator += elapsedTime;

			while (accumulator >= idealDeltaTime) {
				//update
				if (bunnySpawnSystem)
					bunnySpawnSystem->Update(idealDeltaTime, &world, path);
				if (wolfSystem)
					wolfSystem->Update(idealDeltaTime, &world);
				physics.Update(idealDeltaTime, world);
				if (characterController)
					characterController->Update(&world, idealDeltaTime);
				accumulator -= idealDeltaTime;
			}
			if (cameraController)
				cameraController->Update(world);
			renderer.Render(world, window);
			CAudioEngine::instance()->Update();
			if (window.drawGUI && terrain) drawTerrainWindow(window, terrain);
			window.Update();
		}
		state = nextState;
	}
}

void GameController::LoadState() {
	Window::DeleteCallbackDelegates();
	Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)this);

	audio->toggleSound(gameMusic, true);
	world.ClearGameObjects();
	switch (state) {
	case MainMenu:
	{
        gameMusic = audio->PlaySound("back.wav");
		cameraController = new CameraController();
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)cameraController);

		//load sign
		EntityFactory::createTitle(&world);

		break;
	}
	case Level1:
	{
		cameraController = new CameraController();
		characterController = new CharacterController();
		physicsController = new PhysicsController();
		terrainController = new TerrainController();
		bunnySpawnSystem = new BunnySpawnSystem();
		wolfSystem = new WolfSystem();
		treeSystem = new TreeSystem();

      
		gameMusic = audio->PlaySound("back.wav");
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)cameraController);
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)physicsController);
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)terrainController);
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)bunnySpawnSystem);
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)characterController);


		world.mainCamera = EntityFactory::createMainCamera(&world);
		world.mainCharacter = EntityFactory::upgradeCharacter(&world, world.mainCamera);

        //Create skybox
        GameObject *skybox = EntityFactory::createSkybox(&world, resourceDir);
        
		// Create terrain
		terrain = EntityFactory::createTerrain(&world, resourceDir, SIMPLEX_TERRAIN, 1081, glm::vec3(0,80, 0));
		terrain->transform->SetScale(glm::vec3(1, 1, 1));
		
		//create skybox
		skybox = EntityFactory::createSkybox(&world, resourceDir);

		// Place game objects
		//Create Path
		path = EntityFactory::createPath(&world, terrain, 4);

		EntityFactory::createSphere(&world, 2.0, glm::vec3(5, 20, 2.0), 4.0);
		EntityFactory::createSphere(&world, 2.0, glm::vec3(5, 15, 2.0), 4.0);
		EntityFactory::createSphere(&world, 2.0, glm::vec3(5, 10, 2.0), 4.0);

		// Create Physics Ground (below previous ground)
		//EntityFactory::createCube(&world, glm::vec3(groundSize, 0.1, groundSize), glm::vec3(5.5, -4, 2.0), 0);

		// Create boulders
		randomlyPopulateWithBoulders();

		// Create trees
		treeSystem->Spawn(&world);

		EntityFactory::createHUD(&world);
		EntityFactory::createChargeBar(&world);

		break;
	}
	case Level2:
	{
		break;
	}
	case Level3:
	{
		break;
	}
	default:
	{
		break;
	}
	}
}


void GameController::randomlyPopulateWithBoulders() {
	for (int i = 0; i < 15; i++) {
		int type = rand() % 3;
		float scale = rand() % 4 + 1;
		GameObject *boulder = EntityFactory::createBoulder(&world, type, 1);
		float posX = (rand() % (int)groundSize) - groundSize / 2;
		float posZ = (rand() % (int)groundSize) - groundSize / 2;
		boulder->transform->SetPosition(glm::vec3(posX, -4, posZ));
		boulder->transform->SetRotation(glm::vec3(0, rand() % 360, 0));
		boulder->transform->SetScale(glm::vec3(scale, scale, scale));
	}
}

void GameController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
	if (state == MainMenu) {
		if (key == GLFW_KEY_ENTER && action == GLFW_PRESS) {
			nextState = Level1;
            audio->toggleSound(gameMusic, true);
		}
	}
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		nextState = Close;
}
void GameController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
}
void GameController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {
} 
void GameController::MouseScrolled(World *world, double dx, double dy) {
}


