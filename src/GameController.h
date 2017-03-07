#pragma once
#ifndef GameController_h
#define GameController_h

#include <cmath>

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
#include "Terrain.h"
#include "BunnySpawnSystem.h"
#include "WolfSystem.h"
#include "AudioEngine.h"
#include "TreeSystem.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "Time.h"
#include "ImguiUpdateDelegate.h"

enum State {
	Close = -2,
	EndScreen = -1,
	MainMenu = 0,
	Level1 = 1,
	Level2 = 2,
	Level3 = 3,
};

class GameController : WindowCallbackDelegate, ImguiUpdateDelegate {
public:
	GameController() : window(&world, 1080, 920) { };
	virtual ~GameController() {};
	State state;
	State nextState;
	void Init(std::string);
	void Run();
	void LoadState();
	void UnloadState();
	void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action);
	void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY);
	void MouseClicked(World *world, double mouseX, double mouseY, int key, int action);
	void MouseScrolled(World *world, double dx, double dy);
    void ImguiUpdate(World *world);
	void randomlyPopulateWithBoulders();
	int incrState();
	int endState();
private:
	glm::vec2 mousePrev = { 0, 0 };
	std::string resourceDir;
	World world;
	Physics physics;
	Window window;
	Renderer renderer;
	CAudioEngine *audio;
	CameraController *cameraController = nullptr;
	CharacterController *characterController = nullptr;
	PhysicsController *physicsController = nullptr;
	TerrainController *terrainController = nullptr;
	BunnySpawnSystem *bunnySpawnSystem = nullptr;
	WolfSystem *wolfSystem = nullptr;
	TreeSystem *treeSystem = nullptr;
	GameObject *path = nullptr;
	GameObject *terrain = nullptr;
//	GameObject *skybox = nullptr;
    GameObject *sign = nullptr;
	float groundSize = 100.0f;
	int gameMusic = 0;
    
    void displayStats(float deltaTime, World &world, Physics &physics);
    void drawImGUIStuff(Window &window, GameObject *terrain);
    void drawTerrainWindow(Window &window, GameObject *terrain);
};

#endif /* CharacterController_h */
