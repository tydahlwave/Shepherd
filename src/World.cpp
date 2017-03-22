//
//  World.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#include "EntityFactory.h"
#include "Components/RigidBody.h"


World::World() :gameObjects() {
    mainCamera = EntityFactory::createMainCamera(this);
    mainCamera->transform->SetPosition(glm::vec3(0, 0, 0));
    
    ///collision configuration contains default setup for memory, collision setup. Advanced users can create their own configuration.
    btDefaultCollisionConfiguration* collisionConfiguration = new btDefaultCollisionConfiguration();
    
    ///use the default collision dispatcher. For parallel processing you can use a diffent dispatcher (see Extras/BulletMultiThreaded)
    btCollisionDispatcher* dispatcher = new	btCollisionDispatcher(collisionConfiguration);
    
    ///btDbvtBroadphase is a good general purpose broadphase. You can also try out btAxis3Sweep.
    btBroadphaseInterface* overlappingPairCache = new btDbvtBroadphase();
    
    ///the default constraint solver. For parallel processing you can use a different solver (see Extras/BulletMultiThreaded)
    btSequentialImpulseConstraintSolver* solver = new btSequentialImpulseConstraintSolver;
    
    dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher,overlappingPairCache,solver,collisionConfiguration);
    
    dynamicsWorld->setGravity(btVector3(0,-100,0));
}


std::vector<GameObject*> World::GetGameObjects() {
    return gameObjects;
}

GameObject *World::CreateGameObject(std::string name) {
    GameObject *gameObject = new GameObject(name);
    gameObject->world = this;
    gameObjects.push_back(gameObject);
    return gameObject;
}

void World::ClearGameObjects() {
	for (GameObject *go : gameObjects) {
        RemoveGameObject(go);
	}
	gameObjects.clear();
}

void World::RemoveGameObject(GameObject *go) {
    auto it = std::find(gameObjects.begin(), gameObjects.end(), go
                        );
    if(it != gameObjects.end()){
        //remove all physics
        RigidBody *rb = (RigidBody *)((*it)->GetComponent("RigidBody"));
        if(rb) {
            if (rb->bulletRigidBody)
                dynamicsWorld->removeRigidBody(rb->bulletRigidBody);
            rb->bulletRigidBody = nullptr;
        }
        gameObjects.erase(it);
    }
}

