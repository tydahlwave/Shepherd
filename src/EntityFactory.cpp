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
#include "Components/PathRenderer.h"
#include "Components/SkyboxRenderer.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "Terrain.h"
#include  "Path.h"
//#include <Bullet3Geometry>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include <random>

GameObject *EntityFactory::createMainCamera(World *world) {
    GameObject *gameObject = world->CreateGameObject("Camera");
    RigidBody *rigidBody = (RigidBody*)gameObject->AddComponent("RigidBody");
    rigidBody->isKinematic = true;
    gameObject->AddComponent("Camera");
    gameObject->AddComponent("BoxCollider");
    gameObject->transform->SetScale(glm::vec3(3, 3, 3));
    return gameObject;
}

GameObject *EntityFactory::upgradeCharacter(World *world, GameObject *camera) {
	camera->AddComponent("Character");
	MeshRenderer *meshRenderer = (MeshRenderer*)camera->AddComponent("MeshRenderer");
	meshRenderer->model = ModelLibrary::player;
	meshRenderer->shader = ShaderLibrary::cell;
	meshRenderer->material = MaterialLibrary::pearl;
	
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(0, 0, 0));
    btBoxShape* collisionShape = new btBoxShape(btVector3(meshRenderer->model->bounds.halfwidths.x*camera->transform->GetScale().x, meshRenderer->model->bounds.halfwidths.y*camera->transform->GetScale().y, meshRenderer->model->bounds.halfwidths.z*camera->transform->GetScale().z));

	btVector3 inertia(0, 0, 0);
	float mass = 100.0f;
	if (mass != 0)
		collisionShape->calculateLocalInertia(mass, inertia);
	btMotionState* motion = new btDefaultMotionState(t);
	btRigidBody::btRigidBodyConstructionInfo info(mass, motion, collisionShape);
	RigidBody *rigidBody = (RigidBody*)camera->GetComponent("RigidBody");
	rigidBody->isKinematic = false;
	rigidBody->useGravity = true;
	rigidBody->bulletRigidBody = new btRigidBody(info);
	rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    rigidBody->bulletRigidBody->setCollisionFlags(0);
	((Camera*)camera->GetComponent("Camera"))->stat = false;
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
    meshRenderer->model = ModelLibrary::sheep;
    meshRenderer->shader = ShaderLibrary::cell;
    meshRenderer->material = MaterialLibrary::pearl;
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
    gameObject->AddComponent("Death");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->model = ModelLibrary::wolf;
    meshRenderer->shader = ShaderLibrary::cell;
    meshRenderer->material = MaterialLibrary::ruby;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0, 0, 0));
    btBoxShape* collisionShape = new btBoxShape(btVector3(meshRenderer->model->bounds.halfwidths.x, meshRenderer->model->bounds.halfwidths.y, meshRenderer->model->bounds.halfwidths.z));
    btVector3 inertia(0,0,0);
    float mass = 1.0f;
    if(mass != 0)
        collisionShape->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, collisionShape);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createTitle(World *world) {
	GameObject *gameObject = world->CreateGameObject("Title");
	MeshRenderer *mesh = (MeshRenderer*)gameObject->AddComponent("MeshRenderer");
	mesh->model = ModelLibrary::title;
	mesh->shader = ShaderLibrary::menu;
	mesh->material = MaterialLibrary::brass;
	gameObject->transform->SetPosition(glm::vec3(0.f, 0.f, 2.f));
	gameObject->transform->SetRotation(glm::vec3(0.f, 180.f, 0.f));

	return gameObject;
}

GameObject *EntityFactory::createHUD(World *world) {
    GameObject *gameObject = world->CreateGameObject("HUD");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->model = ModelLibrary::cube;
    meshRenderer->shader = ShaderLibrary::hud;
    return gameObject;
}

GameObject *EntityFactory::createChargeBar(World *world) {
    GameObject *gameObject = world->CreateGameObject("ChargeBar");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->model = ModelLibrary::cube;
    meshRenderer->shader = ShaderLibrary::chargeBar;
    return gameObject;
}

//GameObject *EntityFactory::createTexturedSphere(World *world) {
//    GameObject *gameObject = world->CreateGameObject("TexSphere");
//    gameObject->AddComponent("RigidBody");
//    gameObject->AddComponent("BoxCollider");
//    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
//    meshRenderer->mesh = Mesh::sphere_textured;
//    meshRenderer->shader = Shader::textured;
//    meshRenderer->texture = Texture::hair;
//    // IMPORTANT: Must link texture with shader for it to render correctly!!
//    meshRenderer->shader->program->addTexture(meshRenderer->texture->texture);
//    return gameObject;
//}

GameObject *EntityFactory::createGround(World *world) {
    GameObject *gameObject = world->CreateGameObject("Ground");
    gameObject->AddComponent("BoxCollider");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->model = ModelLibrary::plane;
    meshRenderer->shader = ShaderLibrary::phong;
    meshRenderer->material = MaterialLibrary::bronze;
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
    meshRenderer->model = ModelLibrary::cube;
    meshRenderer->shader = ShaderLibrary::phong;
    meshRenderer->material = MaterialLibrary::grass;
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
    meshRenderer->model = ModelLibrary::sphere;
    meshRenderer->shader = ShaderLibrary::cell;
    meshRenderer->material = MaterialLibrary::polishedGold;
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
    meshRenderer->model = ModelLibrary::plane;
    meshRenderer->shader = ShaderLibrary::phong;
    meshRenderer->material = MaterialLibrary::emerald;
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
    int randNum = rand() % 3;
    meshRenderer->model = (randNum <= 1) ? (randNum <= 0) ? ModelLibrary::boulder1 : ModelLibrary::boulder2 : ModelLibrary::boulder3;
//    meshRenderer->model = ModelLibrary::sphere;
    meshRenderer->shader = ShaderLibrary::phong;
    meshRenderer->material = MaterialLibrary::brass;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0, 0, 0));
    btSphereShape* sphere = new btSphereShape(radius);
    btVector3 inertia(0,0,0);
    float mass = 10.0;
    if (mass != 0)
        sphere->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    rigidBody->bulletRigidBody->setFriction(1.f);
    rigidBody->bulletRigidBody->setRollingFriction(0.3f);
    rigidBody->bulletRigidBody->setAnisotropicFriction(sphere->getAnisotropicRollingFrictionDirection(),btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
//    rigidBody->bulletRigidBody->setCollisionFlags(0); // Make it a static object
    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createTerrain(World *world, std::string resourceDir, int type, int size, glm::vec3 pos) {
    GameObject *gameObject = world->CreateGameObject("Terrain");
    TerrainRenderer *renderer = (TerrainRenderer*) gameObject->AddComponent("TerrainRenderer");
    renderer->terrain = new Terrain();
    renderer->terrain->size = size;
    renderer->terrain->type = type;
    renderer->terrain->GenerateFromImage(resourceDir + "terrain9.png");
    renderer->shader = ShaderLibrary::cell;
    renderer->material = MaterialLibrary::bronze;
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));
    btHeightfieldTerrainShape* collisionShape = new btHeightfieldTerrainShape(size, size,
                                                                              renderer->terrain->heightMapFlat.data(), 1.0f,
                                                                              -255.0f, 255.0f, // min/max heights
                                                                              1, PHY_FLOAT,
                                                                              false);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, collisionShape);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createPath(World *world, int size) {
	GameObject *gameObject = world->CreateGameObject("Path");
	PathRenderer *renderer = (PathRenderer*)gameObject->AddComponent("PathRenderer");
    
	renderer->path = new Path();
	renderer->path->size = 6;
	renderer->path->radius = 5;
//	renderer->path->AddNode(glm::vec3(-30, 0, -30));
//	renderer->path->AddNode(glm::vec3(-30, 0, 30));
//    renderer->path->AddNode(glm::vec3(30, 0, 30));
//    renderer->path->AddNode(glm::vec3(0, 0, 0));
//	renderer->path->AddNode(glm::vec3(30, 0, -30));
    renderer->path->AddNode(glm::vec3(-30, 0, -30));
    renderer->path->AddNode(glm::vec3(-30, 0, -100));
    renderer->path->AddNode(glm::vec3(30, 0, -200));
    renderer->path->AddNode(glm::vec3(100, 0, -300));
    renderer->path->AddNode(glm::vec3(200, 0, -400));
    renderer->path->AddNode(glm::vec3(300, 0, -500));
    return gameObject;
}

GameObject *EntityFactory::createTree(World *world, int type, glm::vec3 pos) {
    GameObject *gameObject = world->CreateGameObject("Tree");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
//    meshRenderer->model = (type == 0) ? ModelLibrary::tree1 : (type == 1) ? ModelLibrary::tree2 : ModelLibrary::tree3;
    meshRenderer->model = (type == 0) ? ModelLibrary::tree2 : ModelLibrary::tree3;
    meshRenderer->shader = ShaderLibrary::cell;
    meshRenderer->material = MaterialLibrary::grass;
    gameObject->AddComponent("BoxCollider");
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    rigidBody->isKinematic = true;
	//rigidBody->isPushPullable = false;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));
    btBoxShape* collisionShape = new btBoxShape(btVector3(meshRenderer->model->bounds.halfwidths.x, meshRenderer->model->bounds.halfwidths.y, meshRenderer->model->bounds.halfwidths.z));
//    btVector3 inertia(0,0,0);
    float mass = 0;
//    if (mass != 0)
//        collisionShape->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, collisionShape);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
//    rigidBody->bulletRigidBody->setFriction(1.f);
//    rigidBody->bulletRigidBody->setRollingFriction(1.0f);
//    rigidBody->bulletRigidBody->setSpinningFriction(1.0f);
    rigidBody->bulletRigidBody->setCollisionFlags(0); // Make it a static object

    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createSkybox(World *world, std::string resourceDir) {
	GameObject *gameObject = world->CreateGameObject("Skybox");
	SkyboxRenderer *renderer = (SkyboxRenderer*)gameObject->AddComponent("SkyboxRenderer");
	renderer->skybox = new Skybox(resourceDir);
	renderer->model = ModelLibrary::cube;
	renderer->shader = ShaderLibrary::skybox;

	return gameObject;
}
