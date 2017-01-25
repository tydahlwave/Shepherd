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
