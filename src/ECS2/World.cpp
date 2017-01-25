//
//  World.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#include "World.h"
#include "EntityFactory.h"

World::World() :gameObjects() {
    mainCamera = EntityFactory::createMainCamera(this);
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
