//
//  TreeSpawnSystem.h
//  Shepherd
//
//  Created by Tyler Dahl on 2/15/17.
//
//

#ifndef TreeSystem_h
#define TreeSystem_h

#include <vector>

#include "World.h"

class TreeSystem {
public:
    TreeSystem() {}
    TreeSystem(int _maxTreeCount) :maxTreeCount(_maxTreeCount) {}
    
    int maxTreeCount = 5;
    
    void Spawn(World *world);
private:
    std::vector<GameObject*> trees;
};

#endif /* TreeSpawnSystem_h */
