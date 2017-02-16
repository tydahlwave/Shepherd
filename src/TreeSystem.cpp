//
//  TreeSystem.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 2/15/17.
//
//

#include "TreeSystem.h"

#include "EntityFactory.h"

void TreeSystem::Spawn(World *world) {
    srand(time(0));
    for (int i = 0; i < maxTreeCount; i++) {
        int type = rand() % 3;
        glm::vec3 randPos(rand() % 50 - 25, 0, rand() % 50 - 25);
        GameObject *tree = EntityFactory::createTree(world, type, randPos);
        
        tree->transform->SetPosition(randPos);
        tree->transform->SetRotation(glm::vec3(0, rand() % 360, 0));
        tree->transform->SetScale(glm::vec3(10, 10, 10));
        
        trees.push_back(tree);
    }
}
