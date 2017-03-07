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
#include "Components/HUDRenderer.h"
#include "Components/RigidBody.h"
#include "Components/TerrainRenderer.h"
#include "Components/PathRenderer.h"
#include "Components/SkyboxRenderer.h"
#include "Components/Light.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "TextureLibrary.h"
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
    rigidBody->bulletRigidBody->setFriction(1);
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
    meshRenderer->material = MaterialLibrary::brown;
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

GameObject *EntityFactory::createStaticObject(World *world, std::string name, Model *model, Shader *shader) {
    GameObject *gameObject = world->CreateGameObject(name);
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->model = model;
    meshRenderer->shader = shader;
    return gameObject;
}

GameObject *EntityFactory::createTitle(World *world) {
	GameObject *gameObject = world->CreateGameObject("Title");
	MeshRenderer *mesh = (MeshRenderer*)gameObject->AddComponent("MeshRenderer");
	mesh->model = ModelLibrary::title;
	mesh->shader = ShaderLibrary::menu;
	gameObject->transform->SetPosition(glm::vec3(0.f, .5f, 2.f));
	gameObject->transform->SetRotation(glm::vec3(0.f, 180.f, 0.f));

	return gameObject;
}

GameObject *EntityFactory::createHUD2(World *world) {
	GameObject *gameObject = world->CreateGameObject("HUD2");
	HUDRenderer *hd = (HUDRenderer*)gameObject->AddComponent("HudRenderer");
	gameObject->AddComponent("Clickable");
	hd->model = ModelLibrary::button;
	hd->shader = ShaderLibrary::hud2;
	//gameObject->transform->SetPosition(glm::vec3(.5f, .5f, 0.f)); //moves to center
	//gameObject->transform->SetScale(glm::vec3(100.f, 50.f, 00.f)); //makes 100px wide / 50px tall

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
    gameObject->isSerializable = true;
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

GameObject *EntityFactory::createBoulder(World *world, int boulderType, float radius, vec3 position) {
    GameObject *gameObject = world->CreateGameObject("Boulder");
    gameObject->isSerializable = true;
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    rigidBody->useGravity = true;
    rigidBody->isKinematic = true;
    gameObject->AddComponent("BoxCollider");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    int randNum = rand() % 3;
    meshRenderer->model = (randNum <= 1) ? (randNum <= 0) ? ModelLibrary::boulder1 : ModelLibrary::boulder2 : ModelLibrary::boulder3;
//    meshRenderer->model = ModelLibrary::sphere;
    meshRenderer->shader = ShaderLibrary::cell;
    meshRenderer->material = MaterialLibrary::chrome;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(position.x,position.y,position.z));
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
    NoiseProperties terrainProps;
    terrainProps.frequency = 3.0f;
    terrainProps.octaveHeight = 55.0f;
    time_t seed = 0;
    renderer->terrain->GenerateHeightmap(terrainProps, seed);
    renderer->terrain->UpdateBuffers();
    renderer->terrain->init();
//    renderer->terrain->Generate();
//    renderer->terrain->GenerateFromImage(resourceDir + "terrain9.png");
    renderer->shader = ShaderLibrary::ground;
    renderer->material = MaterialLibrary::grass;
    renderer->textures.push_back(TextureLibrary::grass);
    renderer->textures.push_back(TextureLibrary::mountain);
    renderer->textures.push_back(TextureLibrary::snow);
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));
    btHeightfieldTerrainShape* collisionShape = new btHeightfieldTerrainShape(size, size,
                                                                              renderer->terrain->flattenHeightMap().data(), 1.0f,
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

void EntityFactory::UpdateTerrain(World *world, GameObject *terrainObj, Terrain *terrain) {
    RigidBody *rigidBody = (RigidBody*) terrainObj->GetComponent("RigidBody");
    if (rigidBody) {
        world->dynamicsWorld->removeRigidBody(rigidBody->bulletRigidBody);
        
        btHeightfieldTerrainShape* collisionShape = new btHeightfieldTerrainShape(terrain->size, terrain->size,
                                                                                  terrain->flattenHeightMap().data(), 1.0f,
                                                                                  -255.0f, 255.0f, // min/max heights
                                                                                  1, PHY_FLOAT,
                                                                                  false);
        btTransform t;
        t.setIdentity();
        t.setOrigin(btVector3(terrainObj->transform->GetPosition().x, terrainObj->transform->GetPosition().y, terrainObj->transform->GetPosition().z));
        btMotionState* motion = new btDefaultMotionState(t);
        btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, collisionShape);
        rigidBody->bulletRigidBody = new btRigidBody(info);
        rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
        
        world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    }
}

float getTerrainHeightForPosition(GameObject *terrainObject, Terrain *terrain, int x, int z) {
    glm::vec3 terrainSize = terrainObject->transform->GetScale() * glm::vec3(terrain->size, 1, terrain->size);
    glm::vec3 terrainPos = terrainObject->transform->GetPosition();
    glm::vec3 terrainMin = terrainObject->transform->GetPosition() - terrainSize/2.0f;
//    glm::vec3 terrainMax = terrainObject->transform->GetPosition() + terrainSize/2.0f;
    
    return terrainPos.y + terrain->getHeight((int)((x - terrainMin.x) / terrainObject->transform->GetScale().x), (int)((z - terrainMin.z) / terrainObject->transform->GetScale().z)) * terrainObject->transform->GetScale().y + 90;
}

GameObject *EntityFactory::createPath(World *world, GameObject *terrainObject, int size) {
	GameObject *gameObject = world->CreateGameObject("Path");
	PathRenderer *renderer = (PathRenderer*)gameObject->AddComponent("PathRenderer");
    
    TerrainRenderer *terrainRenderer = (TerrainRenderer*)terrainObject->GetComponent("TerrainRenderer");
    Terrain *terrain = terrainRenderer->terrain;

	renderer->path = new Path();
	renderer->path->size = 3;
	renderer->path->radius = 5;
//	renderer->path->AddNode(glm::vec3(-30, 0, -30));
//	renderer->path->AddNode(glm::vec3(-30, 0, 30));
//    renderer->path->AddNode(glm::vec3(30, 0, 30));
//    renderer->path->AddNode(glm::vec3(0, 0, 0));
//	renderer->path->AddNode(glm::vec3(30, 0, -30));
    renderer->path->AddNode(glm::vec3(-30, getTerrainHeightForPosition(terrainObject, terrain, -30, -30), -30));
    renderer->path->AddNode(glm::vec3(-30, getTerrainHeightForPosition(terrainObject, terrain, -30, -100), -100));
    renderer->path->AddNode(glm::vec3(30, getTerrainHeightForPosition(terrainObject, terrain, 30, -200), -200));
//    renderer->path->AddNode(glm::vec3(100, getTerrainHeightForPosition(terrainObject, terrain, 100, -300), -300));
//    renderer->path->AddNode(glm::vec3(200, getTerrainHeightForPosition(terrainObject, terrain, 200, -400), -400));
//    renderer->path->AddNode(glm::vec3(300, getTerrainHeightForPosition(terrainObject, terrain, 300, -500), -500));
    return gameObject;
}

GameObject *EntityFactory::createTree(World *world, int type, glm::vec3 pos) {
    GameObject *gameObject = world->CreateGameObject("Tree");
    gameObject->isSerializable = true;
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
//    meshRenderer->model = (type == 0) ? ModelLibrary::tree1 : (type == 1) ? ModelLibrary::tree2 : ModelLibrary::tree3;
    meshRenderer->model = (type == 0) ? ModelLibrary::tree2 : ModelLibrary::tree3;
    meshRenderer->shader = ShaderLibrary::cell;
    meshRenderer->material = MaterialLibrary::emerald;
    gameObject->AddComponent("BoxCollider");
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    rigidBody->isKinematic = true;
	//rigidBody->isPushPullable = false;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(pos.x, pos.y, pos.z));
    btBoxShape* collisionShape = new btBoxShape(btVector3(meshRenderer->model->bounds.halfwidths.x, meshRenderer->model->bounds.halfwidths.y, meshRenderer->model->bounds.halfwidths.z));
//    btVector3 inertia(0,0,0);
    float mass = 0.0f;
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

GameObject *EntityFactory::createLight(World *world, glm::vec3 position, bool isDirectional, glm::vec3 intensities, float attenuation,float ambientCoefficient,float coneAngle, glm::vec3 coneDirection) {
    GameObject *gameObject = world->CreateGameObject("Light");
    gameObject->isSerializable = true;
    gameObject->transform->SetPosition(position);
    Light *light = (Light*)gameObject->AddComponent("Light");
    light->isDirectional = isDirectional;
    light->intensities = intensities;
    light->attenuation = attenuation;
    light->ambientCoefficient = ambientCoefficient;
    light->coneAngle = coneAngle;
    light->coneDirection = coneDirection;
    return gameObject;
}
