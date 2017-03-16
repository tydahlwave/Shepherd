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
#include "Components/SheepDestination.h"
#include "Components/TextName.h"
#include "Components/ParticleRenderer.h"
#include "Components/WaterRenderer.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "TextureLibrary.h"
#include "Terrain.h"
#include  "Path.h"
#include "BoneAnimation.h"
#include "Components/Animation.h"
#include "RandomNames.h"
//#include <Bullet3Geometry>
#include <BulletCollision/CollisionShapes/btHeightfieldTerrainShape.h>

#include <random>

GameObject *EntityFactory::createMainCamera(World *world) {
    GameObject *gameObject = world->CreateGameObject("Camera");
    RigidBody *rigidBody = (RigidBody*)gameObject->AddComponent("RigidBody");
    rigidBody->isKinematic = true;
    gameObject->AddComponent("Camera");
    gameObject->AddComponent("BoxCollider");
    gameObject->transform->SetScale(glm::vec3(.002, .002, .002));
    gameObject->transform->SetRotation(glm::vec3(180,0,0));
    return gameObject;
}

GameObject *EntityFactory::upgradeCharacter(World *world, GameObject *camera, glm::vec3 pos) {
	camera->AddComponent("Character");
	MeshRenderer *meshRenderer = (MeshRenderer*)camera->AddComponent("MeshRenderer");
    meshRenderer->model = ModelLibrary::player;
    meshRenderer->shader = ShaderLibrary::anim;
	meshRenderer->material = MaterialLibrary::pearl;
    Animation* comp = (Animation*) camera->AddComponent("Animation");
    comp->skeleton = ModelLibrary::player->skeleton;
    
	btTransform t;
	t.setIdentity();
	t.setOrigin(btVector3(pos.x, pos.y, pos.z));
    btCollisionShape* collisionShape = new btBoxShape(btVector3(meshRenderer->model->bounds.halfwidths.x*camera->transform->GetScale().x, meshRenderer->model->bounds.halfwidths.y*camera->transform->GetScale().y, meshRenderer->model->bounds.halfwidths.z*camera->transform->GetScale().z));

	btVector3 inertia(0, 0, 0);
	float mass = 10.0f;
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
    rigidBody->bulletRigidBody->setRollingFriction(1);
    rigidBody->bulletRigidBody->setCollisionFlags(0);
	((Camera*)camera->GetComponent("Camera"))->stat = false;
    ((Camera*)camera->GetComponent("Camera"))->aap = 90;
	world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    
    camera->transform->SetPosition(pos);
    
    SheepDestination *dest = (SheepDestination*)camera->AddComponent("SheepDestination");
    dest->path = new Path();
    dest->path->size = 1;
    dest->path->radius = 5;
    dest->path->AddNode(pos);
    
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
    //meshRenderer->model = ModelLibrary::sphere;
    meshRenderer->shader = ShaderLibrary::cell;
    meshRenderer->material = MaterialLibrary::pearl;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0, 0, 0));
    btCollisionShape* sphere = new btBoxShape(btVector3(meshRenderer->model->bounds.halfwidths.x, meshRenderer->model->bounds.halfwidths.y, meshRenderer->model->bounds.halfwidths.z));
    btVector3 inertia(0,0,0);
    float mass = 2.0f;
    if(mass != 0)
        sphere->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    
    TextName *name = (TextName*)gameObject->AddComponent("TextName");
    name->name = RandomNames::getRandomName();
    return gameObject;
}

//test anim creature
GameObject *EntityFactory::createTestAnim(World *world) {
    GameObject *gameObject = world->CreateGameObject("Monster");
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    rigidBody->useGravity = true;
    //    rigidBody->isKinematic = true;
    gameObject->AddComponent("BoxCollider");
    Animation* comp = (Animation*) gameObject->AddComponent("Animation");
    //gameObject->AddComponent("TextName");
    comp->skeleton = ModelLibrary::monster->skeleton;
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->model = ModelLibrary::monster;
    meshRenderer->shader = ShaderLibrary::cell;
    meshRenderer->material = MaterialLibrary::pearl;
    //gameObject->transform->SetScale(glm::vec3(0.01f, .01f, .01f));
    gameObject->transform->SetPosition(glm::vec3(-2000, 4, -344));
    
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(0, 20, 0));
    btBoxShape* sphere = new btBoxShape(btVector3(10,10,10));
    btVector3 inertia(0,0,0);
    float mass = 0.0f;
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
//    meshRenderer->texture = TextureLibrary::hair;
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

GameObject *EntityFactory::createStaticObject(World *world, std::string name, Model *model, Shader *shader, Material *material) {
    GameObject *gameObject = world->CreateGameObject(name);
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->model = model;
    meshRenderer->shader = shader;
    meshRenderer->material = material;
    return gameObject;
}

GameObject *EntityFactory::createTitle(World *world) {
	GameObject *gameObject = world->CreateGameObject("Title");
	MeshRenderer *mesh = (MeshRenderer*)gameObject->AddComponent("MeshRenderer");
	mesh->model = ModelLibrary::title;
	mesh->shader = ShaderLibrary::menu;
    mesh->material = MaterialLibrary::emerald;
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

GameObject *EntityFactory::createBarrier(World *world, glm::vec3 position, float size) {
    GameObject *gameObject = world->CreateGameObject("Barrier");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->model = ModelLibrary::cube;
    meshRenderer->shader = ShaderLibrary::phong;
    meshRenderer->material = MaterialLibrary::emerald;
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
    rigidBody->isKinematic = false;
    btTransform t;
    t.setIdentity();
    t.setOrigin(btVector3(position.x,position.y,position.z));
    btBoxShape* shape = new btBoxShape(btVector3(size/2.0,size/2.0,0));
    gameObject->transform->SetScale(vec3(size/2.0,size/2.0,0));
    btVector3 inertia(0,0,0);
    float mass = 0.0;
    if(mass != 0)
        shape->calculateLocalInertia(mass, inertia);
    btMotionState* motion = new btDefaultMotionState(t);
    btRigidBody::btRigidBodyConstructionInfo info(mass, motion, shape, inertia);
    rigidBody->bulletRigidBody = new btRigidBody(info);
    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
    
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
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
    meshRenderer->texture = TextureLibrary::mountain;
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
    NoiseProperties terrainProps;
    terrainProps.frequency = 3.0f;
    terrainProps.octaveHeight = 55.0f;
    terrainProps.seed = 0;
//    renderer->terrain = new Terrain(size, terrainProps);
//    renderer->terrain = new Terrain(resourceDir + "Heightmap.png");
    renderer->terrain = new Terrain(resourceDir + "Level_Heightmap", true);
    renderer->terrain->loadTextureFromFile(resourceDir + "Level_Texturemap.png");
//    renderer->terrain->setTextureFromHeightData();
    renderer->terrain->createMesh();
    renderer->terrain->init();
    renderer->shader = ShaderLibrary::ground;
    renderer->material = MaterialLibrary::grass;
    renderer->textures.push_back(TextureLibrary::grass);
    renderer->textures.push_back(TextureLibrary::mountain);
    renderer->textures.push_back(TextureLibrary::snow);
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
//    btTransform t;
//    t.setIdentity();
//    t.setOrigin(btVector3(pos.x, pos.y, pos.z));
//
//    int dataSize = renderer->terrain->width*renderer->terrain->height;
//    unsigned short *data = (unsigned short *)renderer->terrain->getHeightmap();
//    vector<float> newData(dataSize);
//    for (int i = 0; i < dataSize; i++) {
//        newData.push_back((data[i]-renderer->terrain->min) / SHRT_MAX * renderer->terrain->maxHeight);
//    }
//    
//    btHeightfieldTerrainShape* collisionShape = new btHeightfieldTerrainShape(renderer->terrain->width, renderer->terrain->height,
//                                                                              newData.data(), 1.0f,
//                                                                              0.0f, renderer->terrain->maxHeight, // min/max heights
//                                                                              1, PHY_FLOAT,
//                                                                              false);
//    btMotionState* motion = new btDefaultMotionState(t);
//    btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, collisionShape);

//    rigidBody->bulletRigidBody = new btRigidBody(info);
//    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
//    
//    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

void EntityFactory::UpdateTerrain(World *world, GameObject *terrainObj, Terrain *terrain) {
    RigidBody *rigidBody = (RigidBody*) terrainObj->GetComponent("RigidBody");
    if (rigidBody) {
        world->dynamicsWorld->removeRigidBody(rigidBody->bulletRigidBody);

        btHeightfieldTerrainShape* collisionShape = new btHeightfieldTerrainShape(terrain->size, terrain->size,
                                                                                  terrain->getHeightmap(), 1.0f,
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

float EntityFactory::getTerrainHeightForPosition(GameObject *terrainObject, Terrain *terrain, int x, int z) {
    glm::vec3 terrainSize = terrainObject->transform->GetScale() * glm::vec3(terrain->size, 1, terrain->size);
    glm::vec3 terrainPos = terrainObject->transform->GetPosition();
    glm::vec3 terrainMin = terrainObject->transform->GetPosition() - terrainSize/2.0f;
//    glm::vec3 terrainMax = terrainObject->transform->GetPosition() + terrainSize/2.0f;
    
    return terrainPos.y + terrain->getHeight((int)((x - terrainMin.x) / terrainObject->transform->GetScale().x), (int)((z - terrainMin.z) / terrainObject->transform->GetScale().z)) * terrainObject->transform->GetScale().y + 20;
}

GameObject *EntityFactory::createPath(World *world, GameObject *terrainObject, int size) {
	GameObject *gameObject = world->CreateGameObject("Path");
	PathRenderer *renderer = (PathRenderer*)gameObject->AddComponent("PathRenderer");
    
    TerrainRenderer *terrainRenderer = (TerrainRenderer*)terrainObject->GetComponent("TerrainRenderer");
    Terrain *terrain = terrainRenderer->terrain;

	renderer->path = new Path();
	renderer->path->size = 5;
	renderer->path->radius = 5;
    renderer->path->AddNode(glm::vec3(-220, getTerrainHeightForPosition(terrainObject, terrain, -220, 500), 500));
    renderer->path->AddNode(glm::vec3(-124, getTerrainHeightForPosition(terrainObject, terrain, -124, 210), 210));
    renderer->path->AddNode(glm::vec3(27, getTerrainHeightForPosition(terrainObject, terrain, 27, 150), 150));
    renderer->path->AddNode(glm::vec3(140, getTerrainHeightForPosition(terrainObject, terrain, 140, -97), -97));
    renderer->path->AddNode(glm::vec3(30, getTerrainHeightForPosition(terrainObject, terrain, 30, -200), -200));
    
    for (glm::vec3 pos : renderer->path->GetNodes()) {
        EntityFactory::createLight(world, pos, false, glm::vec3(0.5, 0.5, 0.5), 0.1, 0.15, 50, glm::vec3(0, -1, 0));
    }
    
    SheepDestination *dest = (SheepDestination*)gameObject->AddComponent("SheepDestination");
    dest->path = renderer->path;
    
    return gameObject;
}

GameObject *EntityFactory::createPath(World *world, GameObject *terrainObject, std::vector<glm::vec3> positions) {
    GameObject *gameObject = world->CreateGameObject("Path");
    PathRenderer *renderer = (PathRenderer*)gameObject->AddComponent("PathRenderer");
    
    TerrainRenderer *terrainRenderer = (TerrainRenderer*)terrainObject->GetComponent("TerrainRenderer");
    Terrain *terrain = terrainRenderer->terrain;
    
    renderer->path = new Path();
    renderer->path->size = positions.size();
    renderer->path->radius = 5;
    
    for (glm::vec3 pos : positions) {
        renderer->path->AddNode(glm::vec3(pos.x, getTerrainHeightForPosition(terrainObject, terrain, pos.x, pos.z), pos.z));
    }
    
    SheepDestination *dest = (SheepDestination*)gameObject->AddComponent("SheepDestination");
    dest->path = renderer->path;
    
    return gameObject;
}

GameObject *EntityFactory::createTree(World *world, int type, glm::vec3 pos) {
    GameObject *gameObject = world->CreateGameObject("Tree");
    gameObject->isSerializable = true;
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    switch (type) {
        case 0:
            meshRenderer->model = ModelLibrary::tree1;
            break;
        case 1:
            meshRenderer->model = ModelLibrary::tree2;
            break;
        case 2:
            meshRenderer->model = ModelLibrary::tree3;
            break;
        case 3:
            meshRenderer->model = ModelLibrary::tree4;
            break;
        case 4:
            meshRenderer->model = ModelLibrary::tree5;
            break;
        default:
            meshRenderer->model = ModelLibrary::tree5;
            break;
    }
//    meshRenderer->model = (type == 0) ? ModelLibrary::tree1 : (type == 1) ? ModelLibrary::tree2 : ModelLibrary::tree3;
//    meshRenderer->model = (type == 0) ? ModelLibrary::tree2 : ModelLibrary::tree3;
    meshRenderer->shader = ShaderLibrary::cell;
    meshRenderer->material = MaterialLibrary::emerald;
    meshRenderer->texture = TextureLibrary::grass;
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

GameObject *EntityFactory::createStartMenuTerrain(World *world, std::string resourceDir, int type, int size, glm::vec3 pos) {
    GameObject *gameObject = world->CreateGameObject("StartMenuTerrain");
    TerrainRenderer *renderer = (TerrainRenderer*) gameObject->AddComponent("TerrainRenderer");
    NoiseProperties terrainProps;
    terrainProps.seed = 0;
    terrainProps.frequency = 2.0f;
    terrainProps.octaveHeight = 30.0f;
    renderer->terrain = new Terrain(size, terrainProps);
//    renderer->terrain = new Terrain(resourceDir + "Heightmap.png");
    renderer->terrain->createMesh();
    renderer->terrain->init();
    renderer->shader = ShaderLibrary::ground;
    renderer->material = MaterialLibrary::grass;
    renderer->textures.push_back(TextureLibrary::grass);
    renderer->textures.push_back(TextureLibrary::mountain);
    renderer->textures.push_back(TextureLibrary::snow);
    RigidBody *rigidBody = (RigidBody*) gameObject->AddComponent("RigidBody");
//    btTransform t;
//    t.setIdentity();
//    t.setOrigin(btVector3(pos.x, pos.y, pos.z));
//    btHeightfieldTerrainShape* collisionShape = new btHeightfieldTerrainShape(size, size,
//                                                                              renderer->terrain->getHeightmap(), 1.0f,
//                                                                              -255.0f, 255.0f, // min/max heights
//                                                                              1, PHY_FLOAT,
//                                                                              false);
//    btMotionState* motion = new btDefaultMotionState(t);
//    btRigidBody::btRigidBodyConstructionInfo info(0.0, motion, collisionShape);
//    rigidBody->bulletRigidBody = new btRigidBody(info);
//    rigidBody->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
//    
//    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    return gameObject;
}

GameObject *EntityFactory::createNodeSphere(World *world) {
    // get some values to use
    float radius = 1.0;
    float mass = 1.0;
    glm::vec3 rot = world->mainCharacter->transform->GetRotation();
    float theta = glm::radians(rot.y);
    vec3 camLookAt = vec3(sin(theta) * 1, 0, cos(theta) * 1);
    vec3 position = world->mainCharacter->transform->GetPosition() + 5.0f*camLookAt;
    btVector3 velocityVector = btVector3(camLookAt.x,camLookAt.y,camLookAt.z);
    velocityVector.setY(0.7);
    velocityVector.normalize();
    velocityVector = 100.0*velocityVector;
    
    GameObject *gameObject = world->CreateGameObject("FollowSphere");
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
    rigidBody->bulletRigidBody->setLinearVelocity(velocityVector);
    world->dynamicsWorld->addRigidBody(rigidBody->bulletRigidBody);
    
    if(world->sheepDestinationObject && world->sheepDestinationObject->name == "FollowSphere") {
        world->RemoveGameObject(world->sheepDestinationObject);
        world->sheepDestinationObject = nullptr;
    }
    //world->sheepDestinationObject = gameObject;
    
    SheepDestination *dest = (SheepDestination*)gameObject->AddComponent("SheepDestination");
    dest->path = new Path();
    dest->path->size = 1;
    dest->path->radius = 5;
    dest->path->AddNode(position);
    
    return gameObject;
}

GameObject *EntityFactory::createParticleSystem(World *world, std::string name, int num, float scale, float life, float systemLife, float speed, glm::vec3 position) {
	GameObject *gameObject = world->CreateGameObject("ParticleSystem");
	ParticleRenderer *renderer = (ParticleRenderer*)gameObject->AddComponent("ParticleRenderer");
	renderer->particleSystem = new ParticleSystem(name, num, scale, life, systemLife, speed, position);
	if (name.compare("Snow") == 0) {
		renderer->texture = TextureLibrary::snowflake;
		renderer->particleSystem->setPositionRange(glm::vec2(-800.0f, 800.0f), glm::vec2(-150.0f, 50.0f), glm::vec2(-800.0f, 800.0f));
		renderer->particleSystem->setVelocityRange(glm::vec2(0.0f, 0.0f), glm::vec2(-10.0f, -10.0f), glm::vec2(0.0f, 0.0f));
		renderer->particleSystem->hasTexture = 1.0f;
	}
	else if (name.compare("Blood") == 0) {
		renderer->texture = TextureLibrary::blood;
		renderer->particleSystem->setPositionRange(glm::vec2(-0.75f, 0.75f), glm::vec2(-1.0f, 1.0f), glm::vec2(-0.75f, 0.75f));
		renderer->particleSystem->setVelocityRange(glm::vec2(-1.0, 1.0f), glm::vec2(-1.0f, -0.5f), glm::vec2(-1.0f, 1.0f));
		renderer->particleSystem->hasTexture = 1.0f;
	}

	renderer->shader = ShaderLibrary::particle;

	return gameObject;
}

GameObject *EntityFactory::createWater(World *world, float x, float z, float height) {
	GameObject *gameObject = world->CreateGameObject("Water");
	WaterRenderer *renderer = (WaterRenderer*)gameObject->AddComponent("WaterRenderer");
	renderer->waterTile = new WaterTile(x, z, height);
	renderer->waterTile->tileSize = 50;
	renderer->waterTile->reflectionPlane = glm::vec4(0.0f, 1.0f, 0.0f, -height);
	renderer->waterTile->refractionPlane = glm::vec4(0.0f, -1.0f, 0.0f, height);
	renderer->model = ModelLibrary::boulder3;
	renderer->shader = ShaderLibrary::water;
	renderer->dudv = TextureLibrary::dudvMap;
	renderer->normal = TextureLibrary::normalMap;
	renderer->buffers = new WaterFrameBuffers();
	gameObject->transform->SetPosition(glm::vec3(x, height, z));
	gameObject->transform->SetScale(glm::vec3(renderer->waterTile->tileSize, height, renderer->waterTile->tileSize));
	return gameObject;
}

GameObject *EntityFactory::createRing(World *world) {
    GameObject *gameObject = world->CreateGameObject("Ring");
    MeshRenderer *meshRenderer = (MeshRenderer*) gameObject->AddComponent("MeshRenderer");
    meshRenderer->model = ModelLibrary::ring;
    meshRenderer->shader = ShaderLibrary::phong;
    meshRenderer->material = MaterialLibrary::chrome;
    
    gameObject->transform->SetPosition(world->mainCharacter->transform->GetPosition());
    return gameObject;

}