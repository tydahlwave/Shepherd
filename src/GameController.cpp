
#include <iostream>
#include <ctime>

#include "Mesh.h"
#include "TextureLibrary.h"
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
#include "Components/Light.h"
#include "Terrain.h"
#include "BunnySpawnSystem.h"
#include "WolfSystem.h"
#include "AudioEngine.h"
#include "TreeSystem.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "Time.h"
#include "Components/MeshRenderer.h"
#include "Components/PathRenderer.h"
#include "Serializer.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"
#include "Path.h"

void GameController::displayStats(float deltaTime, World &world, Physics &physics) {
	static float elapsedTime = 0.5;
	static long frames = 0;
	elapsedTime += deltaTime;
	frames++;
	if (elapsedTime > 1.0) {
		elapsedTime = 0;
		int bunnyCount = 0;
        int bunniesAtEnd = 0;
		int groundedObjectsCount = 0;
		GameObject *ground = nullptr;
        Path *path = nullptr;
		for (GameObject *gameObject : world.GetGameObjects()) {
			if (gameObject->name.compare("Ground")) {
				ground = gameObject;
				break;
			}
            PathRenderer *pathRenderer = (PathRenderer*)gameObject->GetComponent("PathRenderer");
            if (pathRenderer && pathRenderer->path) {
                path = pathRenderer->path;
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
                    
                    // Check if bunny is at end
                    if (path) {
                        glm::vec3 endPoint = path->GetNodes()[path->size-1];
                        if (glm::distance(gameObject->transform->GetPosition(), endPoint) < 20.0f) {
                            bunniesAtEnd++;
                        }
                    }
				}
			}
		}
		std::cout << "\nFPS: " << frames << std::endl;
		std::cout << "Game Objects: " << world.GetGameObjects().size() << std::endl;
		std::cout << "Sheep: " << bunnyCount << std::endl;
//		std::cout << "Objects on Ground: " << groundedObjectsCount << std::endl;
//		std::cout << "Bunnies Collected: " << physics.bunniesCollected << std::endl;
        
        if (nextState == Level1 && (bunnyCount == 0 || bunniesAtEnd == bunnyCount)) {
//            nextState = MainMenu;
            std::cout << "Game Over" << std::endl;
            exit(0);
        }
        
		frames = 0;
	}
}


bool show_test_window = true;
bool show_another_window = true;
ImVec4 clear_color = ImColor(114, 144, 154);

void GameController::drawImGUIStuff(Window &window, GameObject *terrain) {
    
    // Prevent gui from being drawn in wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    ImGui_ImplGlfwGL3_NewFrame();
    
    if(terrain) drawTerrainWindow(window, terrain);
    drawLevelEditor(window);
    ImGui::Render();
    
    // Revert wireframe mode to how it was
    glPolygonMode(GL_FRONT_AND_BACK, window.drawWireframes ? GL_LINE : GL_FILL);
}

void GameController::drawTerrainWindow(Window &window, GameObject *terrain) {
    TerrainRenderer *terrainRenderer = (TerrainRenderer*)terrain->GetComponent("TerrainRenderer");
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
		ImGui::Image((void*)(size_t)terrainRenderer->texture->texID, ImVec2(128, 128), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		ImGui::End();
	}

}

void GameController::drawLevelEditor(Window &window) {

    {
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Object Creation Tool");
        static GameObject *mostRecentlyPlacedGameObject = nullptr;
        static int item = 0;
        static float position[3] = { 0.f, 0.f, 0.f };
        static float rotation[3] = { 0.f, 0.f, 0.f };
        static float scale = 1.f;
        static int type = 0;
        static bool isStatic = false;
        
        //for lights
        static bool isDirectional = false;
        static float intensities[3] = { 1.f, 1.f, 1.f };
        static float attenuation = 0.1;
        static float ambientCoefficient = 0.15;
        static float coneAngle = 360;
        static float coneDirection[3] = { 0.f, 1.f, 0.f };
        
        const char* items[] = {"Sphere", "Boulder", "Tree", "Light"};
        if (ImGui::Combo("Objects", &item, items, 4)) {
            
        }
        if (ImGui::Button("Add Item") && !mostRecentlyPlacedGameObject) {
            if(item == 0) {
                mostRecentlyPlacedGameObject = EntityFactory::createSphere(&world, 2, vec3(0), 2);
            }
            if(item == 1) {
                type = rand() % 3;
                mostRecentlyPlacedGameObject = EntityFactory::createBoulder(&world, type, 1, vec3(0));
            }
            if(item == 2) {
                type = (rand() % 2) + 1;
                mostRecentlyPlacedGameObject = EntityFactory::createTree(&world, type, vec3(0));
            }
            if(item == 3) {
                mostRecentlyPlacedGameObject = EntityFactory::createLight(&world, vec3(0), isDirectional, vec3(intensities[0],intensities[1],intensities[2]), attenuation, ambientCoefficient, coneAngle, vec3(coneDirection[0],coneDirection[1],coneDirection[2]));
            }
            //set transform
            mostRecentlyPlacedGameObject->transform->SetPosition(vec3(position[0],position[1],position[2]));
            mostRecentlyPlacedGameObject->transform->SetRotation(vec3(rotation[0],rotation[1],rotation[2]));
            mostRecentlyPlacedGameObject->transform->SetScale(glm::vec3(scale));
            //remove all physics
            RigidBody *rb = (RigidBody *)mostRecentlyPlacedGameObject->GetComponent("RigidBody");
            if(rb) rb->bulletRigidBody = nullptr;
        }
        if(item == 3) {
            // Light menu stuff
            ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
            ImGui::Begin("Light Creation Tool");
            
            if(ImGui::Checkbox("isDirectional", &isDirectional) && mostRecentlyPlacedGameObject) {
                Light *light = (Light *)mostRecentlyPlacedGameObject->GetComponent("Light");
                if(light) light->isDirectional = isDirectional;
            };
            if (ImGui::SliderFloat3("Intensities", intensities, 0, 1) && mostRecentlyPlacedGameObject) {
                Light *light = (Light *)mostRecentlyPlacedGameObject->GetComponent("Light");
                if(light) light->intensities = vec3(intensities[0],intensities[1],intensities[2]);
            };
            if (ImGui::SliderFloat("Attenuation", &attenuation, 0, 1.0) && mostRecentlyPlacedGameObject) {
                Light *light = (Light *)mostRecentlyPlacedGameObject->GetComponent("Light");
                if(light) light->attenuation = attenuation;
            };
            if (ImGui::SliderFloat("Cone Angle", &coneAngle, 0, 360) && mostRecentlyPlacedGameObject) {
                Light *light = (Light *)mostRecentlyPlacedGameObject->GetComponent("Light");
                if(light) light->coneAngle = coneAngle;
            };
            if (ImGui::SliderFloat3("Cone Direction", coneDirection, -1, 1) && mostRecentlyPlacedGameObject) {
                Light *light = (Light *)mostRecentlyPlacedGameObject->GetComponent("Light");
                if(light) light->coneDirection = vec3(coneDirection[0],coneDirection[1],coneDirection[2]);
            };
            
            ImGui::End();
        }
        if (ImGui::Button("Finalize Item") && mostRecentlyPlacedGameObject) {
            if(!isStatic) {
                // then add it to world without removing physics
                if(mostRecentlyPlacedGameObject->name == "Sphere") {
                    world.RemoveGameObject(mostRecentlyPlacedGameObject);
                    mostRecentlyPlacedGameObject = EntityFactory::createSphere(&world, 2, vec3(position[0],position[1],position[2]), 2);
                }
                else if(mostRecentlyPlacedGameObject->name == "Boulder") {
                    world.RemoveGameObject(mostRecentlyPlacedGameObject);
                    mostRecentlyPlacedGameObject = EntityFactory::createBoulder(&world, type, 1, vec3(position[0],position[1],position[2]));
                }
                else if(mostRecentlyPlacedGameObject->name == "Tree") {
                    world.RemoveGameObject(mostRecentlyPlacedGameObject);
                    mostRecentlyPlacedGameObject = EntityFactory::createTree(&world, type, vec3(position[0],position[1],position[2]));
                }
                mostRecentlyPlacedGameObject->transform->SetPosition(vec3(position[0],position[1],position[2]));
                mostRecentlyPlacedGameObject->transform->SetRotation(vec3(rotation[0],rotation[1],rotation[2]));
                mostRecentlyPlacedGameObject->transform->SetScale(glm::vec3(scale));
            }
            mostRecentlyPlacedGameObject->SetIsStatic(isStatic);
            mostRecentlyPlacedGameObject = nullptr;
        }
        if (ImGui::Button("Get Current Position")) {
            GameObject *go = world.mainCamera;
            position[0] = go->transform->GetPosition().x;
            position[1] = go->transform->GetPosition().y;
            position[2] = go->transform->GetPosition().z;
        }
        if (ImGui::DragFloat3("Position", position)) {
            if(mostRecentlyPlacedGameObject != nullptr)
                mostRecentlyPlacedGameObject->transform->SetPosition(vec3(position[0],position[1],position[2]));
        }
        if (ImGui::DragFloat3("Rotation", rotation)) {
            if(mostRecentlyPlacedGameObject != nullptr)
                mostRecentlyPlacedGameObject->transform->SetRotation(vec3(rotation[0],rotation[1],rotation[2]));
        }
        if (ImGui::DragFloat("Scale", &scale)) {
            if(mostRecentlyPlacedGameObject != nullptr)
                mostRecentlyPlacedGameObject->transform->SetScale(vec3(scale));
        }
        if (ImGui::Checkbox("Static", &isStatic)) {
            
        }
        ImGui::End();
    }
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
	TextureLibrary::LoadTextures(resourceDir);
	MaterialLibrary::InitializeMaterials();
}

void GameController::Run() {
    audio->LoadSounds(resourceDir);
    nextState = MainMenu;
    state = MainMenu;
    
	while (state != Close) {
		LoadState();
		// Seed random generator
		srand(time(0));

		// Init times
		long oldTime = Time::Now();
		float idealDeltaTime = 1.f / 60.f;
		float accumulator = 0.0f;

		

		//audio->PlaySound("herdAmbient.wav");
        

		// Game loop
		while (state == nextState) {
            sign->transform->SetRotation(vec3(0,180,cos(Time::Now() / 1000.0) * 2));
            //sign->transform->SetPosition(vec3(0,sin(Time::Now() / 2000.0) * .02 + .5 ,2));
			long curTime = Time::Now();
			float elapsedTime = (curTime - oldTime) / 1000.0f;
			// Reset current frame time
			oldTime = curTime;

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
            if(window.drawGUI) drawImGUIStuff(window, terrain);
			window.Update();
            displayStats(elapsedTime, world, physics);
            
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
        
        std::cout<<"play menu";
        
		cameraController = new CameraController();
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)cameraController);

		//load sign
        sign = EntityFactory::createTitle(&world);
        //Create skybox
        //GameObject *skybox = EntityFactory::createSkybox(&world, resourceDir);
        
        /*GameObject *signShad = EntityFactory::createTitle(&world);
        signShad->transform->SetScale(vec3(1,.2,1));
        signShad->transform->SetPosition(vec3(-1.0,-1.0,2.5));
        signShad->RemoveComponent("MeshRenderer");
        MeshRenderer *mesh = (MeshRenderer*)signShad->AddComponent("MeshRenderer");
        mesh->model = ModelLibrary::title;
        mesh->shader = ShaderLibrary::cell;*/
        
        gameMusic = audio->PlaySound("menu.wav");
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

        audio->toggleSound(gameMusic, true);
		gameMusic = audio->PlaySound("back.wav");
        audio->SetChannelvolume(gameMusic, 2);
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
		terrain = EntityFactory::createTerrain(&world, resourceDir, SIMPLEX_TERRAIN, 1081, glm::vec3(0, 0, 0));
		terrain->transform->SetScale(glm::vec3(10, 10, 10));
		
		//create skybox
		skybox = EntityFactory::createSkybox(&world, resourceDir);

		// Place game objects
		//Create Path
		path = EntityFactory::createPath(&world, terrain, 4);

        
        Serializer::DeserializeWorld(&world);
//		EntityFactory::createSphere(&world, 2.0, glm::vec3(5, 20, 2.0), 4.0);
//		EntityFactory::createSphere(&world, 2.0, glm::vec3(5, 15, 2.0), 4.0);
//		EntityFactory::createSphere(&world, 2.0, glm::vec3(5, 10, 2.0), 4.0);
        
		// Create boulders
		//randomlyPopulateWithBoulders();

		// Create trees
		//treeSystem->Spawn(&world);
        
        //Place a single light
        //EntityFactory::createLight(&world, glm::vec3(-4,100,10), false, glm::vec3(1, 1, 1), 0.1f, 0.0f, 360.0f, glm::vec3(0,-1,0));

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
        float posX = (rand() % (int)groundSize) - groundSize / 2;
        float posZ = (rand() % (int)groundSize) - groundSize / 2;
        vec3 position = glm::vec3(posX, -4, posZ);
		GameObject *boulder = EntityFactory::createBoulder(&world, type, 1, position);
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
    if (key == GLFW_KEY_P && action == GLFW_PRESS)
        Serializer::SerializeWorld(world);
}
void GameController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
}
void GameController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {
} 
void GameController::MouseScrolled(World *world, double dx, double dy) {
}


