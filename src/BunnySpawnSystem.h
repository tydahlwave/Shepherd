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
#include "Time.h"
//static const int maxEntities = 20;

class BunnySpawnSystem : WindowCallbackDelegate {
public:
    BunnySpawnSystem() {};
    virtual ~BunnySpawnSystem() {};
    
    glm::vec3 startPosition = glm::vec3(0, 0, 0);
    glm::vec3 endPosition = glm::vec3(0);
    
	void Update(float deltaTime, World *world, GameObject *path);
	void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) override;
	void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) override;
	void MouseClicked(World *world, double mouseX, double mouseY, int key, int action) override;
	void MouseScrolled(World *world, double dx, double dy) override;
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
    vector<GameObject*> bunniesAtEnd;

private:
    static const int maxEntities = 30;
	int count = 0;
    float elapsedTime = 0;
//    float spawnRate = 3.0; // seconds per spawn
	float maxSpeed = 3.5;
	float angle = 0.0;
	bool flockToCamera = false;
//	bool stop = false;
	std::map<GameObject *, int> bunnyNode;
	Path *path;
    void CreateBunny(World *world);
    void jumpAtEndOfLevel(World *world);
    
};

#endif /* BunnySpawnSystem_h */
