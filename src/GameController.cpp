
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
#include "Components/Button.h"
#include "Components/RigidBody.h"
#include "Components/TerrainRenderer.h"
#include "Components/HUDRenderer.h"
#include "Components/Light.h"
#include "Components/SheepDestination.h"
#include "Components/TextName.h"
#include "Terrain.h"
#include "BunnySpawnSystem.h"
#include "WolfSystem.h"
#include "AudioEngine.h"
#include "TreeSystem.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "TextureLoader.h"
#include "Time.h"
#include "Components/MeshRenderer.h"
#include "Components/PathRenderer.h"
#include "Components/Animation.h"
#include "Model.h"
#include "Serializer.h"
#include "Components/Clickable.h"
#include "LevelEditor.h"
#include "Keyframe.h"

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
        //		for (GameObject *gameObject : world.GetGameObjects()) {
        //			if (gameObject->name.compare("Bunny") == 0) {
        //				RigidBody *rigidBody = (RigidBody*)gameObject->GetComponent("RigidBody");
        //				if (rigidBody) {
        //					bunnyCount++;
        //					if (abs(rigidBody->velocity.y) < 0.01) {
        //						groundedObjectsCount++;
        //					}
        //
        //                    // Check if bunny is at end
        //                    if (path) {
        //                        glm::vec3 endPosition = path->GetNodes()[path->size-1];
        //                        if (glm::distance(gameObject->transform->GetPosition(), endPosition) < 20.0f) {
        //                            bunniesAtEnd++;
        //                        }
        //                    }
        //				}
        //			}
        //		}
        std::cout << "\nFPS: " << frames << std::endl;
        std::cout << "Game Objects: " << world.GetGameObjects().size() << std::endl;
        std::cout << "Sheep: " << bunnyCount << std::endl;
        //		std::cout << "Objects on Ground: " << groundedObjectsCount << std::endl;
        //		std::cout << "Bunnies Collected: " << physics.bunniesCollected << std::endl;
        
        //        if (nextState == Level1 && (bunnyCount == 0 || bunniesAtEnd == bunnyCount)) {
        ////            nextState = MainMenu;
        //            std::cout << "Game Over" << std::endl;
        //            exit(0);
        //        }
        
        frames = 0;
    }
}


void GameController::ImGuiShowNames(World *world) {
    //note on making it work for 4k displays!!
    //FROM: http://stackoverflow.com/questions/25230841/how-to-find-display-scaling-factor-on-retina-4k-displays
    //You need to use glfwGetFramebufferSize to get the actual size of the window in pixels on displays that use window coordinates that don't match pixels. Then pass those values to glViewport.
    
    // draw names over sheep
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(1,1,1,0));
    ImGui::PushStyleColor(ImGuiCol_TitleBg, ImVec4(1,1,1,0));
    ImGui::PushStyleColor(ImGuiCol_TitleBgActive, ImVec4(1,1,1,0));
    ImGui::PushStyleColor(ImGuiCol_TitleBgCollapsed, ImVec4(1,1,1,0));
//        ImGuiIO& io = ImGui::GetIO();
//        ImFont* font0 = io.Fonts->AddFontDefault();
//        ImFont* font = io.Fonts->AddFontFromFileTTF("../../resources/fonts/Baloo_Bhaina/BalooBhaina.ttf", 40);
    //    if(!font) cout << "error: counldn't load font\n";
    //    if(!font->IsLoaded()) cout << "font not loaded";
//        ImGui::PushFont(font);
    for (GameObject *gameObject : world->GetGameObjects()) {
        TextName *textName = (TextName*)gameObject->GetComponent("TextName");
        if(textName && Renderer::intersectFrustumAABB((Camera*)world->mainCamera->GetComponent("Camera"), gameObject->getBounds().getMin(), gameObject->getBounds().getMax())) {
            
            GLint viewportArray[4];
            glGetIntegerv(GL_VIEWPORT, viewportArray);
            vec4 viewport = vec4(viewportArray[0], viewportArray[1], viewportArray[2], viewportArray[3]);
            float aspectRatio = (float)window.GetWidth() / (float)window.GetHeight();
            //std::cout<<window.GetWidth() << " :   " <<window.GetHeight() << std::endl;
            Camera *camera = (Camera*)world->mainCamera->GetComponent("Camera");
            mat4 P = glm::perspective(45.0f, aspectRatio, 0.01f, 1000.0f);
            mat4 V = glm::lookAt(camera->pos, camera->lookAt, camera->up);
            
            vec3 projected = glm::project(gameObject->transform->GetPosition(), V, P, viewport);

            
            // now write characters to screen in this projected screen pos
            float distance = glm::distance(world->mainCharacter->transform->GetPosition(), gameObject->transform->GetPosition());
            float alpha = 1.0 - distance*distance / 5000.0;
            ImVec4 textCol = ImVec4(textName->color.x, textName->color.y, textName->color.z, 1);
            ImGui::PushStyleColor(ImGuiCol_Text, textCol);
            
            // define variables for width and height of each imgui name window
            float width = 200;
            float height = 30;
            ImGui::SetNextWindowSize(ImVec2(width,height), ImGuiSetCond_FirstUseEver);
            
            //on retina, change the 1.0f's to 2.0f... and vice versa
            float constant = 1.0f;
            if (window.GetWidth() > 1200) { // this is a very hack fix... but i think it works
                constant = 2.0f;
            }
            ImGui::SetNextWindowPos(ImVec2((projected.x - 10.0)/constant, (window.GetHeight() - (projected.y + 90.0))/constant));
            
            //ImGui::SetNextWindowPos(ImVec2(projected.x, window.GetHeight() - 100));
            //std::cout<< projected.x << " :   " <<projected.y << std::endl;
            //ImGui::SetNextWindowCollapsed(true, ImGuiSetCond_Once);
            ImGui::Begin(textName->name.c_str(), nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs);
            ImGui::Text("%s", textName->name.c_str());
            ImGui::End();
            ImGui::PopStyleColor();
        }
    }
    //    ImGui::PopFont();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
    ImGui::PopStyleColor();
}

void GameController::ImGuiShowHelp(World *world) {
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    if (!world->showHelp) {
        ImGui::Begin("Need Help?", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs);
        ImGui::Text("Press 'H' to show help");
    }
    else {
        ImGui::Begin("Help Window", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs);
        ImGui::Text("Movement - ASDF");
        ImGui::Text("Jump - Space");
        ImGui::Text("Throw follow orb - 1");
        ImGui::Text("Remove follow orb - 2");
        ImGui::Text("Ping to have sheep follow you - F");
        ImGui::Text("Force Push Roar - Left Click");
        ImGui::Text("Force Pull Roar - Right Click");
        //ImGui::SetWindowFontScale(10);
        
    }
    ImGui::End();
}

void GameController::ImGuiShowStats(World *world) {
    ImGui::SetNextWindowPos(ImVec2(100, 100));

    ImGui::Begin("Stats Stuff", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs);
    ImGui::Text("Sheep At End: %lu  ", bunnySpawnSystem->bunniesAtEnd.size());
    unsigned long numOfAliveSheep = 0;
    for (GameObject *go : world->GetGameObjects()) {
        if (go->name == "Bunny" && go->GetComponent("RigidBody")) {
            numOfAliveSheep++;
        }
    }
    ImGui::Text("Sheep Still Alive: %lu  ", numOfAliveSheep);
    ImGui::End();
}

void GameController::ImguiUpdate(World *world, bool drawGUI) {
    //    if(!drawGUI) return;
    if(terrain && drawGUI) drawTerrainWindow(window, terrain);
    if (drawGUI) LevelEditor::drawLevelEditor(window, world);
    ImGuiShowNames(world);
    ImGuiShowHelp(world);
    ImGuiShowStats(world);
}

void GameController::drawTerrainWindow(Window &window, GameObject *terrain) {
    TerrainRenderer *terrainRenderer = (TerrainRenderer*)terrain->GetComponent("TerrainRenderer");
	if (!terrainRenderer)
		return;
//    TextureLoader *textureTest = terrainRenderer->terrain->getTexture();
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
		ImGui::Image((void*)(size_t)terrainRenderer->terrain->heightmapTex.texID, ImVec2(128, 128), uv0, uv1, ImColor(255, 255, 255, 255), ImColor(255, 255, 255, 128));
		ImGui::End();
	}
}

void GameController::checkIfEndOfLevel() {
    int numOfAliveBunnies = 0;
    for(GameObject * gameObject : world.GetGameObjects()) {
        if (gameObject->name != "Bunny") continue;
        RigidBody *rb = (RigidBody*)gameObject->GetComponent("RigidBody");
        if(rb) {
            numOfAliveBunnies++;
            if( glm::distance(bunnySpawnSystem->endPosition, gameObject->transform->GetPosition()) < 50.0f) {
                if(!gameObject->isBunnyAndIsAtEnd) {
                    gameObject->isBunnyAndIsAtEnd = true;
                    bunnySpawnSystem->bunniesAtEnd.push_back(gameObject);
                }
            }
        }
    }
    //cout << "bunnies at end: " << bunnySpawnSystem->bunniesAtEnd.size() << endl;
    if(numOfAliveBunnies == bunnySpawnSystem->bunniesAtEnd.size()){
        //either you won or you lost
        if(bunnySpawnSystem->bunniesAtEnd.size() == 0){
            nextState = Level2;
            return;
        }
        window.DeleteWindowCallbackDelegate((WindowCallbackDelegate*)cameraController);
        window.DeleteWindowCallbackDelegate((WindowCallbackDelegate*)characterController);
        window.DeleteWindowCallbackDelegate((WindowCallbackDelegate*)physicsController);
        wolfSystem = nullptr;
        GameObject *winLevelTitle = EntityFactory::createTitle(&world);
        MeshRenderer *mesh = (MeshRenderer*)winLevelTitle->GetComponent("MeshRenderer");
        mesh->shader = ShaderLibrary::inFrontOfCamera;
        winLevelTitle->transform->SetPosition(glm::vec3(0.f,0.5f,0.f));
        winLevelTitle->transform->SetScale(glm::vec3(1,-1,1)*0.5f);
        winLevelTitle->transform->SetRotation(glm::vec3(180.f, 0.f, 0.f));
        MeshRenderer *meshRenderer = (MeshRenderer*)winLevelTitle->GetComponent("MeshRenderer");
        meshRenderer->model->bounds.halfwidths = vec3(INFINITY);
        levelComplete = true;
    }
}

void GameController::Init(std::string resourceDir) {
    
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
    
    world.resourceDir = this->resourceDir;
    
    while (state != Close) {
        LoadState();
        
        window.checkMouse();
        // Seed random generator
        srand(time(0));
        
        // Init times
        long oldTime = Time::Now();
        float idealDeltaTime = 1.f / 60.f;
        float accumulator = 0.0f;
        
        
        
        //audio->PlaySound("herdAmbient.wav");
        
        
        // Game loop
        while (state == nextState) {
            if (state == MainMenu) sign->transform->SetRotation(vec3(0,180,cos(Time::Now() / 1000.0) * 2));
            //sign->transform->SetPosition(vec3(0,sin(Time::Now() / 2000.0) * .02 + .5 ,2));

			long curTime = Time::Now();
			float elapsedTime = (curTime - oldTime) / 1000.0f;
			// Reset current frame time
			oldTime = curTime;

			accumulator += elapsedTime;

			while (accumulator >= idealDeltaTime) {
				//update
                if(world.sheepDestinationObject && world.sheepDestinationObject->name != "Path") {
                    SheepDestination* sd = (SheepDestination *)world.sheepDestinationObject->GetComponent("SheepDestination");
                    sd->Update();
                }

				if (bunnySpawnSystem)
					bunnySpawnSystem->Update(idealDeltaTime, &world, world.sheepDestinationObject);
				if (wolfSystem)
                    wolfSystem->Update(idealDeltaTime, &world);
				physics.Update(idealDeltaTime, world);
                if (animSystem)
                    animSystem->Update(elapsedTime, &world);
				if (characterController)
                    characterController->Update(&world, idealDeltaTime);
                if(state == Level1 || state == Level2 || state == Level3) {
                    if (!levelComplete) checkIfEndOfLevel();
                }
				accumulator -= idealDeltaTime;
                
			}
			if (cameraController) {
				if (cameraController->Update(world, elapsedTime*1000)) {
					world.RemoveGameObject(world.mainCamera);
					world.mainCamera = world.mainCharacter;
					EntityFactory::createHUD(&world);
				}
			}
			/*
			Camera *c;
			if (nextcamlevel > 0 && camlevel > nextcamlevel && state == Level1) {
				c = (Camera *)world.mainCamera->GetComponent("Camera");
				nextcamlevel += 4.f;
				switch (camstage) {
				case 0:
					c->pos = glm::vec3(-490.233f, 323.772, -547.755);
					c->pitch = -59.f;
					c->aap = 48.f;
					break;
				case 1:
					c->pos = glm::vec3(605.167, 372.280, -199.671);
					c->pitch = -45.f;
					c->aap = 275.f;
					break;
				case 2:
					c->pos = glm::vec3(-628.876, 500.95, 518.358);
					c->pitch = -50.f;
					c->aap = 480.f;
					break;
				case 3:
					world.RemoveGameObject(world.mainCamera);
					world.mainCamera = world.mainCharacter;
					nextcamlevel = 0.f;
                    EntityFactory::createHUD(&world);
                        
//                        GameObject *winLevelTitle = EntityFactory::createTitle(&world);
//                        MeshRenderer *mesh = (MeshRenderer*)winLevelTitle->GetComponent("MeshRenderer");
//                        mesh->shader = ShaderLibrary::inFrontOfCamera;
//                        winLevelTitle->transform->SetPosition(glm::vec3(0.f,0.5f,0.f));
//                        winLevelTitle->transform->SetScale(glm::vec3(1,-1,1)*0.5f);
//                        winLevelTitle->transform->SetRotation(glm::vec3(180.f, 0.f, 0.f));
//                        MeshRenderer *meshRenderer = (MeshRenderer*)winLevelTitle->GetComponent("MeshRenderer");
//                        meshRenderer->model->bounds.halfwidths = vec3(INFINITY);
                        
//                        Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)cameraController, 1);
//                        Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)physicsController, 1);
//                        Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)terrainController, 1);
//                        Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)bunnySpawnSystem, 1);
//                        Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)characterController, 1);

					break;
				}
				camstage++;
			}
			*/
			renderer.Render(world);
			CAudioEngine::instance()->Update();
			window.Update();

            //displayStats(elapsedTime, world, physics);
			
            
        }
        UnloadState();
        state = nextState;
    }
}

void GameController::LoadState() {
	Window::DeleteCallbackDelegates();
	Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)this, 0);

	audio->toggleSound(gameMusic, true);
	world.ClearGameObjects();
	switch (state) {
	case MainMenu:
	{
        
        std::cout<<"play menu";
        
		cameraController = new CameraController();
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)cameraController, 1);

		//load sign
        sign = EntityFactory::createTitle(&world);
        //sign->transform->SetScale(vec3(0));
        
       
        
		//load enterbutton
//		GameObject *startButton = EntityFactory::createHUD2(&world);
//		startButton->transform->SetPosition(glm::vec3(0.5f, 0.7f, 0));
//		startButton->transform->SetScale(glm::vec3(70.f, 30.f, 0.f));
//		Button * bt = (Button *)(startButton->AddComponent("Button"));
//		bt->callback = &GameController::incrState;
//		HUDRenderer *hr = (HUDRenderer *)(startButton->GetComponent("HudRenderer"));
//		hr->texture = TextureLibrary::startButton;
//		
//		GameObject *startButton2 = EntityFactory::createHUD2(&world);
//		startButton2->transform->SetPosition(glm::vec3(0.5f, 0.8f, 0));
//		startButton2->transform->SetScale(glm::vec3(70.f, 30.f, 0.f));
//		bt = (Button *)(startButton2->AddComponent("Button"));
//		bt->callback = &GameController::endState;
//		hr = (HUDRenderer *)(startButton2->GetComponent("HudRenderer"));
//		hr->texture = TextureLibrary::quitButton;
        
        GameObject *playSign = EntityFactory::createStaticObject(&world, "PlaySign", ModelLibrary::playSign, ShaderLibrary::menu);
        playSign->transform->SetScale(glm::vec3(0.4, 0.4, 0.4));
        playSign->transform->SetPosition(glm::vec3(-0.25f, -0.3f, 2.0f));
        playSign->transform->SetRotation(glm::vec3(0.0f, 180.f, 0.0f));
		playSign->AddComponent("Clickable");
		((Button *)(playSign->AddComponent("Button")))->callback = &GameController::incrState;
		
        GameObject *exitSign = EntityFactory::createStaticObject(&world, "ExitSign", ModelLibrary::exitSign, ShaderLibrary::menu);
        exitSign->transform->SetScale(glm::vec3(0.3, 0.3, 0.3));
        exitSign->transform->SetPosition(glm::vec3(0.25f, -0.7f, 2.0f));
        exitSign->transform->SetRotation(glm::vec3(0.0f, 180.f, 0.0f));
		exitSign->AddComponent("Clickable");
		((Button *)(exitSign->AddComponent("Button")))->callback = &GameController::endState;
		//playSign->AddComponent("Clickable");

		/*
		GameObject *startButton3 = EntityFactory::createHUD2(&world);
		startButton3->transform->SetPosition(glm::vec3(0.8f, 0.5f, 0));
		startButton3->transform->SetScale(glm::vec3(70.f, 30.f, 0.f));
		*/
		window.drawMouse = true;

		world.mainCamera = EntityFactory::createMainCamera(&world);
        //Create skybox
        //GameObject *skybox = EntityFactory::createSkybox(&world, resourceDir);
        
        /*GameObject *signShad = EntityFactory::createTitle(&world);
        signShad->transform->SetScale(vec3(1,.2,1));
        signShad->transform->SetPosition(vec3(-1.0,-1.0,2.5));
        signShad->RemoveComponent("MeshRenderer");
        MeshRenderer *mesh = (MeshRenderer*)signShad->AddComponent("MeshRenderer");
        mesh->model = ModelLibrary::title;
        mesh->shader = ShaderLibrary::cell;*/
        
        // Create terrain
        GameObject *startMenuTerrain = EntityFactory::createStartMenuTerrain(&world, resourceDir, SIMPLEX_TERRAIN, 256, glm::vec3(0, -20, 0));
        startMenuTerrain->transform->SetPosition(glm::vec3(0, -20, 0));
      
        
        // Add directional light
        EntityFactory::createLight(&world, glm::vec3(-0.6, 0.8, -1.0), true, glm::vec3(2, 2, 2), 1.0, 0.15, 1.0, glm::vec3(1, 1, 1));
        // Add point light in front of sign
        EntityFactory::createLight(&world, glm::vec3(1.f, .3f, 1.3f), false, glm::vec3(1), 15.0, 0.15, 360., glm::vec3(1));
        
        //Create skybox
        EntityFactory::createSkybox(&world, resourceDir);
        
        //Create Path
        std::vector<glm::vec3> pathPositions = {
            glm::vec3(50, -20, 50),
            glm::vec3(-40, -20, 70),
            glm::vec3(0, -20, 40)
        };
        world.sheepDestinationObject = EntityFactory::createPath(&world, startMenuTerrain, pathPositions);
        
        bunnySpawnSystem = new BunnySpawnSystem();
        bunnySpawnSystem->startPosition = glm::vec3(60, -20, 40);

        
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
        bunnySpawnSystem->startPosition = glm::vec3(-500, 4, -350);
        bunnySpawnSystem->endPosition = glm::vec3(185,241,362);
        //bunnySpawnSystem->endPosition = glm::vec3(-490,5,-316);
		wolfSystem = new WolfSystem();
		treeSystem = new TreeSystem();
        animSystem = new AnimationSystem();
		printf("Loading level 1\n");
        audio->toggleSound(gameMusic, true);
		gameMusic = audio->PlaySound("back.wav");
        audio->SetChannelvolume(gameMusic, 1);
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)cameraController, 1);
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)physicsController, 1);
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)terrainController, 1);
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)bunnySpawnSystem, 1);
		Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)characterController, 1);
        Window::AddImguiUpdateDelegate(this);


		world.mainCamera = EntityFactory::createMainCamera(&world);
		world.mainCharacter = EntityFactory::upgradeCharacter(&world, world.mainCamera,glm::vec3(-500, 4, -350));
        world.cameraController = (GameObject*)cameraController;
        
        // Create fences
        GameObject *fence1 = EntityFactory::createStaticObject(&world, "Fence", ModelLibrary::fence, ShaderLibrary::cell, MaterialLibrary::brown);
        fence1->transform->SetPosition(glm::vec3(185,241,322));
        fence1->transform->SetRotation(glm::vec3(0, 0, 0));
        fence1->transform->SetScale(glm::vec3(40, 40, 40));
        
        GameObject *fence2 = EntityFactory::createStaticObject(&world, "Fence", ModelLibrary::fence, ShaderLibrary::cell, MaterialLibrary::brown);
        fence2->transform->SetPosition(glm::vec3(185,241,402));
        fence2->transform->SetRotation(glm::vec3(0, 0, 0));
        fence2->transform->SetScale(glm::vec3(40, 40, 40));
        
        GameObject *fence3 = EntityFactory::createStaticObject(&world, "Fence", ModelLibrary::fence, ShaderLibrary::cell, MaterialLibrary::brown);
        fence3->transform->SetPosition(glm::vec3(227,241,361));
        fence3->transform->SetRotation(glm::vec3(0, 90, 0));
        fence3->transform->SetScale(glm::vec3(40, 40, 40));
        
//        GameObject *fence4 = EntityFactory::createStaticObject(&world, "Fence", ModelLibrary::fenceWithDoor, ShaderLibrary::cell, MaterialLibrary::brown);
//        fence4->transform->SetPosition(glm::vec3(135,241,362));
//        fence4->transform->SetRotation(glm::vec3(0, 90, 0));
//        fence4->transform->SetScale(glm::vec3(40, 40, 40));

        //Create skybox
        GameObject *skybox = EntityFactory::createSkybox(&world, resourceDir);
        
		// Create terrain
		terrain = EntityFactory::createTerrain(&world, resourceDir, SIMPLEX_TERRAIN, 541, glm::vec3(0, 0, 0));
		terrain->transform->SetScale(glm::vec3(5, 5, 5));
		
		//create skybox
		skybox = EntityFactory::createSkybox(&world, resourceDir);

		// Place game objectsaw
		//Create Path
        world.sheepDestinationObject = nullptr;//world.mainCharacter;
        
        Serializer::DeserializeWorld(&world);
//		EntityFactory::createSphere(&world, 2.0, glm::vec3(5, 20, 2.0), 4.0);
//		EntityFactory::createSphere(&world, 2.0, glm::vec3(5, 15, 2.0), 4.0);
//		EntityFactory::createSphere(&world, 2.0, glm::vec3(5, 10, 2.0), 4.0);
        
		// Create boulders
//        PathRenderer *p = (PathRenderer*)path->GetComponent("PathRenderer");
//		randomlyPopulateWithBoulders(p->path);

		// Create trees
		//treeSystem->Spawn(&world);
//        EntityFactory::createBarrier(&world, glm::vec3(200,100,60), 350);

        
        
        GameObject* test = EntityFactory::createTestAnim(&world);
        Animation* testAnim = (Animation*) test->GetComponent("Animation");
        testAnim->anim = true;
        
        BoneAnimation Anim_Test_Walk = *new BoneAnimation("Walk", FramesToTime(glm::vec2(0,30)), 2);
        //testAnim->skeleton.StopAnimating();
        testAnim->skeleton.SetIdleAnimation(&Anim_Test_Walk);
        //The true is for loop, and the false is for reset_to_start.
        testAnim->skeleton.PlayAnimation(Anim_Test_Walk,true,false);
//
//        for(Bone b : testAnim->skeleton.bones)
//        {
//            std::cout<<"Bone "<<b.name<<std::endl;
//        }
        
		//treeSystem->Spawn(&world);
        
        //Place a single light
        //EntityFactory::createLight(&world, glm::vec3(-4,100,10), false, glm::vec3(1, 1, 1), 0.1f, 0.0f, 360.0f, glm::vec3(0,-1,0));

        // Add directional light
        EntityFactory::createLight(&world, glm::vec3(1, 1, 1), true, glm::vec3(1, 1, 1), 1.0, 0.15, 1.0, glm::vec3(1, 1, 1));
//		EntityFactory::createHUD(&world);
		EntityFactory::createChargeBar(&world);
		world.mainCamera = EntityFactory::createMainCamera(&world);
        
        
		Camera * c = (Camera *)world.mainCamera->GetComponent("Camera");
		Keyframe ks;
		c->pos = ks.pos = glm::vec3(-490.233f, 323.772, -547.755);
		c->pitch = ks.pitch = -59.f;
		c->aap = ks.aap = 48.f;
		ks.time = 5000.f;
		c->kfs.add(ks);
		ks.pos = glm::vec3(605.167, 372.280, -199.671);
		ks.pitch = -45.f;
		ks.aap = 275.f-360.f;
		ks.time = 5000.f;
		c->kfs.add(ks); 
		ks.pos = glm::vec3(-628.876, 500.95, 518.358);
		ks.pitch = -50.f;
		ks.aap = 120.f;
		ks.time = 5000.f;
		c->kfs.add(ks);
//		ks.time = 1000.f;
//		c->kfs.hold(1000);
        break;
	}
	case Level2:
	{
        nextState = Level1;
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

void GameController::UnloadState() {
    Window::DeleteCallbackDelegates();
    Window::AddWindowCallbackDelegate((WindowCallbackDelegate*)this, 0);
    
    audio->toggleSound(gameMusic, true);
    world.ClearGameObjects();
    if (bunnySpawnSystem) {
        delete bunnySpawnSystem;
        bunnySpawnSystem = nullptr;
    }
    if (wolfSystem) {
        delete wolfSystem;
        wolfSystem = nullptr;
    }
    if (characterController) {
        delete characterController;
        characterController = nullptr;
    }
    if (terrainController) {
        delete terrainController;
        terrainController = nullptr;
    }
    if (physicsController) {
        delete physicsController;
        physicsController = nullptr;
    }
    if (treeSystem) {
        delete treeSystem;
        treeSystem = nullptr;
    }
    if (animSystem) {
        delete animSystem;
        animSystem = nullptr;
    }
    window.drawMouse = false;
}


void GameController::randomlyPopulateWithBoulders(Path *path) {
    //	for (int i = 0; i < 15; i++) {
    //		int type = rand() % 3;
    //		float scale = rand() % 4 + 1;
    //        float posX = (rand() % (int)groundSize) - groundSize / 2;
    //        float posZ = (rand() % (int)groundSize) - groundSize / 2;
    //        vec3 position = glm::vec3(posX, -4, posZ);
    //		GameObject *boulder = EntityFactory::createBoulder(&world, type, 1, position);
    //		boulder->transform->SetRotation(glm::vec3(0, rand() % 360, 0));
    //		boulder->transform->SetScale(glm::vec3(scale, scale, scale));
    //	}
    
    std::vector<glm::vec3> nodes = path->GetNodes();
    for (int i = 0; i < nodes.size(); i++) {
        for (int i = 0; i < 5; i++) {
            int type = rand() % 3;
            float scale = rand() % 4 + 1;
            float posX = (rand() % (int)groundSize) - groundSize / 2;
            float posZ = (rand() % (int)groundSize) - groundSize / 2;
            vec3 position = nodes[i] + glm::vec3(posX, -20, posZ);
            GameObject *boulder = EntityFactory::createBoulder(&world, type, 1, position);
            boulder->transform->SetRotation(glm::vec3(0, rand() % 360, 0));
            boulder->transform->SetScale(glm::vec3(scale, scale, scale));
        }
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
    if (key == GLFW_KEY_F1 && action == GLFW_PRESS)
        nextState = MainMenu;
    if (key == GLFW_KEY_ENTER && action == GLFW_PRESS){
        cout << "I PRESSED ENTER" << endl;
        camstage = 3;
    }
}
void GameController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
    //printf("mousemoving!");
}
void GameController::MouseClicked(World *world2, double mouseX, double mouseY, int key, int action) {
    if (window.drawMouse == true) {
        int id = renderer.checkClickable(world, mouseX, mouseY);
        for (GameObject *go : world.GetGameObjects()) {
            Clickable *cl = (Clickable *)go->GetComponent("Clickable");
            if (!cl || cl->id != id)
                continue;
            Button *b = (Button *)go->GetComponent("Button");
            if (!b || !b->callback)
                continue;
            ButtonFunc bf = b->callback;
            (this->*bf)();
        }
    }
} 
void GameController::MouseScrolled(World *world, double dx, double dy) {
}

int GameController::incrState() {
    nextState = static_cast<State>(state + 1);
    return nextState;
}

int GameController::endState() {
    nextState = State::Close;
    return nextState;
}
