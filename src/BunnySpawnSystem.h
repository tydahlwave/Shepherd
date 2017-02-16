//
//  BunnySpawnSystem.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/24/17.
//
//

#ifndef BunnySpawnSystem_h
#define BunnySpawnSystem_h

#include <iostream>
#include <random>
#include "World.h"
#include "WindowCallbackDelegate.h"
#include <GLFW/glfw3.h>
#include "Path.h"
//static const int maxEntities = 20;

class BunnySpawnSystem : WindowCallbackDelegate {
public:
    BunnySpawnSystem() {};
    virtual ~BunnySpawnSystem() {};
    
	void Update(float deltaTime, World *world, GameObject *path);
	void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) override;
	void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) override;
	void MouseClicked(World *world, double mouseX, double mouseY, int key, int action) override;
	void Flock(World *world, GameObject *bunny, glm::vec3 target);
	glm::vec3 Seek(World *world, GameObject *bunny, glm::vec3 target);
	void Arrival(World *world, GameObject *bunny, glm::vec3 target);
	void Separation(GameObject *bunny);
	void Alignment(GameObject *bunny);
	void Cohesion(GameObject *bunny);
	glm::vec3 FollowPath(World *world, GameObject *bunny);
	bool LineIntersectsObj(glm::vec3 ahead, glm::vec3 ahead2, GameObject *obstacle);
	GameObject *FindClosestObstacle(World *world, GameObject *bunny, glm::vec3 ahead, glm::vec3 ahead2);
	void ObstacleAvoidance(World *world);

private:
    static const int maxEntities = 45;
	int count = 0;
    float elapsedTime = 0;
    float spawnRate = 3.0; // seconds per spawn
	float maxSpeed = 3.5;
	float angle = 0.0;
	bool flockToCamera = false;
	bool stop = false;
	std::map<GameObject *, int> bunnyNode;
	Path *path;
};

#endif /* BunnySpawnSystem_h */
