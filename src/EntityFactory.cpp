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
#include "Components/TerrainRenderer.h"
#include "Terrain.h"
#include "Components/PathRenderer.h"
#include  "Path.h"

#include <random>

GameObject *EntityFactory::createMainCamera(World *world) {
    GameObject *gameObject = world->CreateGameObject("Camera");
    RigidBody *rigidBody = (RigidBody*)gameObject->AddComponent("RigidBody");
    rigidBody->isKinematic = true;
    gameObject->AddComponent("Camera");
    gameObject->AddComponent("BoxCollider");
    gameObject->transform->SetScale(glm::vec3(0.2, 0.2, 0.2));
    return gameObject;
}

GameObject *EntityFactory::upgradeCharacter(World *world, GameObject *camera) {
	camera->AddComponent("Character");
	MeshRenderer *meshRenderer = (MeshRenderer*)camera->AddComponent("MeshRenderer");
	meshRenderer->mesh = Mesh::cube;
	meshRenderer->shader = Shader::phong;
	meshRenderer->material = Material::copper;
	
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(0, 0, 0));
	btSphereShape* sphere = new btSphereShape(1);
	btVector3 inertia(0, 0, 0);
	float mass = 100.0f;
	if (mass != 0)
		sphere->calculateLocalInertia(mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere);
	RigidBody *rigidBody = (RigidBody*)camera->GetComponent("RigidBody");
	rigidBody->isKinematic = false;
	rigidBody->useGravity = true;
	rigidBody->bulletRigidBody = new btRigidBody(info);
	rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);

	world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
	return camera;
}

GameObject *EntityFactory::createBunny(World *world) {
    GameObject *gameObject = world->CreateGameObject("Bunny");
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    rigidBody->useGravity = true;
//    rigidBody->isKinematic = true;
    gameObject->AddComponent("BoxCollider");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->mesh = Mesh::bunny;
    meshRenderer->shader = Shader::phong;
    meshRenderer->material = Material::pearl;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0, 0, 0));
    btSphereShape* sphere = new btSphereShape(1);
    btVector3 inertia(0,0,0);
    float mass = 100.0f;
    if(mass != 0)
        sphere->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createWolf(World *world) {
    GameObject *gameObject = world->CreateGameObject("Wolf");
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    rigidBody->useGravity = true;
//    rigidBody->isKinematic = true;
    gameObject->AddComponent("BoxCollider");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->mesh = Mesh::bunny;
    meshRenderer->shader = Shader::phong;
    meshRenderer->material = Material::ruby;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0, 0, 0));
    btSphereShape* sphere = new btSphereShape(1);
    btVector3 inertia(0,0,0);
    float mass = 1.0f;
    if(mass != 0)
        sphere->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createHUD(World *world) {
    GameObject *gameObject = world->CreateGameObject("HUD");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->mesh = Mesh::cube;
    meshRenderer->shader = Shader::hud;
    return gameObject;
}

GameObject *EntityFactory::createTexturedSphere(World *world) {
    GameObject *gameObject = world->CreateGameObject("TexSphere");
    gameObject->AddComponent("RigidBody");
    gameObject->AddComponent("BoxCollider");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->mesh = Mesh::sphere_textured;
    meshRenderer->shader = Shader::textured;
    meshRenderer->texture = Texture::hair;
    // IMPORTANT: Must link texture with shader for it to render correctly!!
    meshRenderer->shader->program->addTexture(meshRenderer->texture->texture);
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
    meshRenderer->material = Material::grass;
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    rigidBody->isKinematic = false;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(position.x,position.y,position.z));
    btBoxShape* cube = new btBoxShape(btVector3(dimensions.x/2.0,dimensions.y/2.0,dimensions.z/2.0));
    gameObject->transform->SetScale(dimensions/2.0f);
    btVector3 inertia(0,0,0);
    if(mass != 0)
        cube->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, cube, inertia);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    rigidBody->bulletRigidBody->setFriction(1.f);
    rigidBody->bulletRigidBody->setRollingFriction(0.3f);
    rigidBody->bulletRigidBody->setAnisotropicFriction(cube->getAnisotropicRollingFrictionDirection(),btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);

    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createSphere(World *world, float radius, glm::vec3 position, float mass) {
    GameObject *gameObject = world->CreateGameObject("Sphere");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->mesh = Mesh::sphere;
    meshRenderer->shader = Shader::phong;
    meshRenderer->material = Material::polishedGold;
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    rigidBody->isKinematic = true;
    rigidBody->useGravity = true;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(position.x,position.y,position.z));
    btSphereShape* sphere = new btSphereShape(radius);
    gameObject->transform->SetScale(glm::vec3(radius));
    btVector3 inertia(0,0,0);
    if(mass != 0)
        sphere->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere, inertia);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    rigidBody->bulletRigidBody->setFriction(1.f);
    rigidBody->bulletRigidBody->setRollingFriction(0.3f);
    rigidBody->bulletRigidBody->setAnisotropicFriction(sphere->getAnisotropicRollingFrictionDirection(),btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);

    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createPhysicsGround(World *world) {
    GameObject *gameObject = world->CreateGameObject("PhysicsGround");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->mesh = Mesh::plane;
    meshRenderer->shader = Shader::phong;
    meshRenderer->material = Material::emerald;
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0,0,0));
    btStaticPlaneShape* plane = new btStaticPlaneShape(btVector3(0,1,0),0);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, plane);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createBoulder(World *world, int boulderType, float radius) {
    GameObject *gameObject = world->CreateGameObject("Boulder");
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    rigidBody->useGravity = true;
    rigidBody->isKinematic = true;
    gameObject->AddComponent("BoxCollider");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
//    meshRenderer->mesh = (boulderType <= 1) ? (boulderType <= 0) ? Mesh::boulder1 : Mesh::boulder2 : Mesh::boulder3;
    meshRenderer->mesh = Mesh::sphere;
    meshRenderer->shader = Shader::phong;
    meshRenderer->material = Material::brass;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0, 0, 0));
    btSphereShape* sphere = new btSphereShape(radius);
    btVector3 inertia(0,0,0);
    float mass = 0.0;
    if (mass != 0)
        sphere->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    rigidBody->bulletRigidBody->setCollisionFlags(0); // Make it a static object
    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createTerrain(World *world, int type, int size) {
    GameObject *gameObject = world->CreateGameObject("Terrain");
    TerrainRenderer *renderer = (TerrainRenderer*) gameObject->AddComponent("TerrainRenderer");
    renderer->terrain = new Terrain();
    renderer->terrain->size = size;
    renderer->terrain->type = type;
    renderer->terrain->Generate();
    renderer->shader = Shader::phong;
    renderer->material = Material::bronze;
    return gameObject;
}

GameObject *EntityFactory::createPath(World *world, int size) {
	GameObject *gameObject = world->CreateGameObject("Path");
	PathRenderer *renderer = (PathRenderer*)gameObject->AddComponent("PathRenderer");
	renderer->path = new Path();
	renderer->path->size = size;
	renderer->path->radius = 5;
	renderer->path->AddNode(glm::vec3(-30, 0, -30));
	renderer->path->AddNode(glm::vec3(-30, 0, 30));
	renderer->path->AddNode(glm::vec3(30, 0, 30));
	renderer->path->AddNode(glm::vec3(30, 0, -30));
	return gameObject;
}
