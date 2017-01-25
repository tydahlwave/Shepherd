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

class BunnySpawnSystem {
public:
    BunnySpawnSystem() {};
    virtual ~BunnySpawnSystem() {};
    
    void Update(float deltaTime, World *world);
private:
    int maxEntities = 20;
    float elapsedTime = 0;
    float spawnRate = 2.0; // 2 second per spawn
    std::vector<GameObject*> bunnies;
};

#endif /* BunnySpawnSystem_h */
