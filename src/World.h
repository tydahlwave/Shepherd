//
//  World.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef World_h
#define World_h

#include <vector>

class GameObject;
class KDTree;

#include "GameObject.h"
#include "AudioEngine.h"

#include "KDTree.h"
#ifdef WIN32
#include <btBulletDynamicsCommon.h>
#else
#include <BulletDynamics/btBulletDynamicsCommon.h>
#include <BulletCollision/btBulletCollisionCommon.h>
#endif

class World {
public:
    World();
    virtual ~World() {};
    
    GameObject *mainCharacter = nullptr;
	GameObject *mainCamera = nullptr;
    
    btDiscreteDynamicsWorld *dynamicsWorld;
    
    std::vector<GameObject*> GetGameObjects();
    GameObject *CreateGameObject(std::string name);
private:
    std::vector<GameObject*> gameObjects;
    KDTree *kdTree;
};

#endif /* World_h */
