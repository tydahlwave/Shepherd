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

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <sstream>

class World {
public:
    World();
    virtual ~World() {};
    
    GameObject *mainCharacter = nullptr;
	GameObject *mainCamera = nullptr;
    
    btDiscreteDynamicsWorld *dynamicsWorld;
    
    std::vector<GameObject*> GetGameObjects();
    GameObject *CreateGameObject(std::string name);
	void ClearGameObjects();
    void RemoveGameObject(GameObject *go);
    void SerializeWorld();
    void DeserializeWorld();
private:
    std::vector<GameObject*> gameObjects;
    KDTree *kdTree;
    Transform *DeserializeTransform(rapidjson::Value &v);
    void DeserializeSphere(rapidjson::Value &v);
    void DeserializeBoulder(rapidjson::Value &v);
    void DeserializeTree(rapidjson::Value &v);
};

#endif /* World_h */
