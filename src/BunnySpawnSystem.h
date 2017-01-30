//
//  BunnySpawnSystem.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/24/17.
//
//

#ifndef BunnySpawnSystem_h
#define BunnySpawnSystem_h

#include "World.h"
#include "WindowCallbackDelegate.h"
#include <GLFW/glfw3.h>
//static const int maxEntities = 20;

class BunnySpawnSystem : WindowCallbackDelegate {
public:
    BunnySpawnSystem() {};
    virtual ~BunnySpawnSystem() {};
    
	void Update(float deltaTime, World *world);
	void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) override;
	void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY);
	void MouseClicked(World *world, double mouseX, double mouseY, int key, int action);
	void Flock(World *world);
	void Seek(World *world, glm::vec3 target);
	void Arrival(World *world, glm::vec3 target);
	void Separation();
	void Alignment();
	void Cohesion();

private:
    static const int maxEntities = 20;
	int count = 0;
    float elapsedTime = 0;
    float spawnRate = 2.0; // 2 second per spawn
	float maxSpeed = 3.5;
	float angle = 0.0;
	bool flockToCamera = false;
    std::vector<GameObject*> bunnies;
};

#endif /* BunnySpawnSystem_h */
