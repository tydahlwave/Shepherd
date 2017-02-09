//
//  GameObject.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#include <iostream>

#include "GameObject.h"
#include "Components/Camera.h"
#include "Components/MeshRenderer.h"
#include "Components/RigidBody.h"
#include "Components/BoxCollider.h"
#include "Components/SphereCollider.h"
#include "Components/TerrainRenderer.h"
#include "Components/PathRenderer.h"

GameObject::GameObject() :components() {
    name = "GameObject";
    AddComponent("Transform"); // The transform component is required
}

GameObject::GameObject(std::string name) :components() {
    this->name = name;
    AddComponent("Transform"); // The transform component is required
}

GameObject::GameObject(std::string name, std::vector<std::string> componentNames) :GameObject(name) {
    for (std::string componentName : componentNames) {
        AddComponent(componentName);
    }
}

Bounds GameObject::getBounds() {
    MeshRenderer *meshRenderer = (MeshRenderer*) this->GetComponent("MeshRenderer");
    if (meshRenderer) {
        return meshRenderer->model->bounds;
    } else {
        return Bounds().TransformedBounds(this->transform);
    }
}

Component *GameObject::AddComponent(std::string name) {
    if (!components[name]) {
        Component *component;
        if (name.compare("Transform") == 0) {
            transform = new Transform();
            component = (Component*)transform;
        } else if (name.compare("Camera") == 0) {
            component = (Component*) new Camera();
        } else if (name.compare("MeshRenderer") == 0) {
            component = (Component*) new MeshRenderer();
        } else if (name.compare("RigidBody") == 0) {
            component = (Component*) new RigidBody();
        } else if (name.compare("BoxCollider") == 0) {
            component = (Component*) new BoxCollider();
        } else if (name.compare("SphereCollider") == 0) {
            component = (Component*) new SphereCollider();
		}
		else if (name.compare("TerrainRenderer") == 0) {
			component = (Component*) new TerrainRenderer();
		}
		else if (name.compare("PathRenderer") == 0) {
			component = (Component*) new PathRenderer();
        } else {
            component = nullptr;
            std::cout << name << " component does not exist." << std::endl;
        }
        component->gameObject = this;
        components[name] = component;
        return component;
    } else {
        std::cout << name << " component has already been added." << std::endl;
    }
    return nullptr;
}

Component *GameObject::GetComponent(std::string name) {
    return components[name];
}

void GameObject::RemoveComponent(std::string name) {
    components[name] = nullptr;
}

void GameObject::Destroy() {
//    for(std::map<std::string, Component*>::iterator it = components.begin(); it != components.end(); it++) {
        // iterator->first = key
        // iterator->second = value
        // Repeat if you also want to iterate through the second map.
//        if (components[it->first]) {
//            components[it->first]->gameObject = nullptr;
//            components[it->first] = nullptr;
//        }
//        world->GetGameObjects()
//        delete this;
//    }
}
