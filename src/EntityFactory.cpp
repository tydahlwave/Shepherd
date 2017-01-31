//
//  EntityFactory.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#include "EntityFactory.h"
#include "Components/Transform.h"
#include "Components/Collider.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/RigidBody.h"

GameObject *EntityFactory::createMainCamera(World *world) {
    GameObject *gameObject = world->CreateGameObject("MainCamera");
    RigidBody *rigidBody = (RigidBody*)gameObject->AddComponent("RigidBody");
    rigidBody->isKinematic = true;
    gameObject->AddComponent("Camera");
    gameObject->AddComponent("BoxCollider");
    gameObject->transform->scale = glm::vec3(0.2, 0.2, 0.2);
    return gameObject;
}

GameObject *EntityFactory::createMainCharacter(World *world) {
	GameObject *gameObject = world->CreateGameObject("MainCharacter");
	((RigidBody*)gameObject->AddComponent("RigidBody"))->isKinematic = true;
	gameObject->AddComponent("BoxCollider");
	gameObject->AddComponent("Camera");
	MeshRenderer *meshRenderer = (MeshRenderer*)gameObject->AddComponent("MeshRenderer");
	meshRenderer->mesh = Mesh::bunny;
	meshRenderer->shader = Shader::phong;
	meshRenderer->material = Material::emerald;
	return gameObject;
}

GameObject *EntityFactory::createBunny(World *world) {
    GameObject *gameObject = world->CreateGameObject("Bunny");
    gameObject->AddComponent("RigidBody");
    gameObject->AddComponent("BoxCollider");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->mesh = Mesh::bunny;
    meshRenderer->shader = Shader::phong;
    meshRenderer->material = Material::emerald;
    return gameObject;
}

GameObject *EntityFactory::createGround(World *world) {
    GameObject *gameObject = world->CreateGameObject("Ground");
    gameObject->AddComponent("BoxCollider");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->mesh = Mesh::plane;
    meshRenderer->shader = Shader::phong;
    meshRenderer->material = Material::bronze;
    return gameObject;
}

GameObject *EntityFactory::createBarrier(World *world) {
    GameObject *gameObject = world->CreateGameObject("Barrier");
    gameObject->AddComponent("BoxCollider");
    return gameObject;
}

GameObject *EntityFactory::createCube(World *world, glm::vec3 dimensions, glm::vec3 position, float mass) {
    GameObject *gameObject = world->CreateGameObject("Cube");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->mesh = Mesh::cube;
    meshRenderer->shader = Shader::phong;
    meshRenderer->material = Material::ruby;
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(position.x,position.y,position.z));
    btBoxShape* cube = new btBoxShape(btVector3(dimensions.x/2.0,dimensions.y/2.0,dimensions.z/2.0));
    btVector3 inertia(0,0,0);
    if(mass != 0)
        cube->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, cube);
    rigidBody->bulletRigidBody = new btRigidBody(info);

    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createPhysicsGround(World *world) {
    GameObject *gameObject = world->CreateGameObject("PhysicsGround");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->mesh = Mesh::plane;
    meshRenderer->shader = Shader::phong;
    meshRenderer->material = Material::polishedGold;
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0,0,0));
    btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0,1,0),0);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}
