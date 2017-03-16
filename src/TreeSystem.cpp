//
//  TreeSystem.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 2/15/17.
//
//

#include "TreeSystem.h"
#include <time.h>
#include "EntityFactory.h"

typedef struct TreeSpawn {
    bool active = false;
    int type = 0;
    glm::vec3 position = glm::vec3(0, 0, 0);
    glm::vec3 rotation = glm::vec3(0, 0, 0);
    glm::vec3 scale = glm::vec3(1, 1, 1);
    GameObject *tree = nullptr;
} TreeSpawn;

#define SPAWN_REGION_SIZE 101
TreeSpawn spawns[SPAWN_REGION_SIZE][SPAWN_REGION_SIZE] = {};
float density = 0.0005f; // 1.0f = tree in every cell

void TreeSystem::Initialize(World *world) {
    srand(time(0));
    
    for (int row = 0; row < SPAWN_REGION_SIZE; row++) {
        for (int col = 0; col < SPAWN_REGION_SIZE; col++) {
            spawns[row][col].active = (rand() % (int)(1/density) == 0);
            if (spawns[row][col].active) {
                spawns[row][col].position = glm::vec3(rand() % 100 - 50, 0, rand() % 100 - 50);
//                spawns[row][col].position = glm::vec3((col - SPAWN_REGION_SIZE/2) * 5, 0, (row - SPAWN_REGION_SIZE/2) * 5);
                spawns[row][col].rotation = glm::vec3(glm::vec3(0, rand() % 360, 0));
                spawns[row][col].scale = glm::vec3(10, 10, 10);
                spawns[row][col].type = rand() % 3;
                spawns[row][col].tree = EntityFactory::createTree(world, spawns[row][col].type, spawns[row][col].position);
                spawns[row][col].tree->transform->SetPosition(spawns[row][col].position);
                spawns[row][col].tree->transform->SetRotation(spawns[row][col].rotation);
                spawns[row][col].tree->transform->SetScale(spawns[row][col].scale);
                trees.push_back(spawns[row][col].tree);
            }
        }
    }
}
// 300, -500
void TreeSystem::Spawn(World *world) {
    srand(time(0));
    for (int i = 0; i < maxTreeCount; i++) {
        int type = rand() % 6;
        glm::vec3 randPos(rand() % 150 - 50, 0, rand() % 300 - 200);
        GameObject *tree = EntityFactory::createTree(world, type, randPos);
        
        tree->transform->SetPosition(randPos);
        tree->transform->SetRotation(glm::vec3(0, rand() % 360, 0));
        tree->transform->SetScale(glm::vec3(10, 10, 10));
        
        trees.push_back(tree);
    }
}

void TreeSystem::Update(World *world, glm::vec3 playerPos) {
//    int playerColIndex = abs((int)playerPos.x % SPAWN_REGION_SIZE);
//    int playerRowIndex = abs((int)playerPos.z % SPAWN_REGION_SIZE);
    
    // Iterate through all cells, updating tree positions if needed
    for (int row = 0; row < SPAWN_REGION_SIZE; row++) {
        for (int col = 0; col < SPAWN_REGION_SIZE; col++) {
            // If cell contains a tree
            if (spawns[row][col].active) {
                // If tree is outside of region, update it's position
                if (fabsf(spawns[row][col].position.x - playerPos.x) > SPAWN_REGION_SIZE ||
                    fabsf(spawns[row][col].position.z - playerPos.z) > SPAWN_REGION_SIZE) {
//                    int distRowRight = (SPAWN_REGION_SIZE - playerRowIndex) + row;
//                    int distRowLeft = playerRowIndex - row;
//                    int directionRow = (distRowRight > distRowLeft) ? 1 : -1;
//                    int distColRight = (SPAWN_REGION_SIZE - playerColIndex) + col;
//                    int distColLeft = playerColIndex - col;
//                    int directionCol = (distColRight > distColLeft) ? 1 : -1;
                    spawns[row][col].position = glm::vec3(
                            playerPos.x + (playerPos.x - spawns[row][col].position.x), 0,
                            playerPos.z + (playerPos.z - spawns[row][col].position.z));
                    spawns[row][col].tree->transform->SetPosition(spawns[row][col].position);
                }
            }
        }
    }
}
