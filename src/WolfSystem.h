//
//  WolfSystem.h
//  Shepherd
//
//  Created by Tyler Dahl on 1/29/17.
//
//

#ifndef WolfSystem_h
#define WolfSystem_h

#include <vector>
#include "glm/glm.hpp"

class GameObject;
class World;

class WolfSystem {
public:
    WolfSystem() {};
    virtual ~WolfSystem() {};
    
    void Update(float deltaTime, World *world);
private:
    static const int maxEntities = 200;
    float elapsedTime = -10.f;
    float spawnRate = 15.0f; // seconds per spawn
    float maxSpeed = 10.0f;
    std::vector<GameObject*> wolves;
    
    void Spawn(World *world);
    void MoveWolf(GameObject *wolf, float deltaTime, World *world);
    glm::vec3 CalculateSheepCenter(World *world);
    GameObject *GetClosestSheep(GameObject *wolf, World *world);
};

#endif /* WolfSystem_h */
