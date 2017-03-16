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
#include "Components/SkyboxRenderer.h"
#include "Components/Clickable.h"
#include "Components/Button.h"
#include "Components/HUDRenderer.h"
#include "Components/Character.h"
#include "Components/Death.h"
#include "Components/Light.h"
#include "Components/SheepDestination.h"
#include "Components/TextName.h"
#include "Components/ParticleRenderer.h"
#include "Components/WaterRenderer.h"

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

bool GameObject::GetIsStatic() {
    return IsStatic;
}

void GameObject::SetIsStatic(bool isStatic) {
    if(isStatic) {
        IsStatic = true;
        RigidBody *rb = (RigidBody*)GetComponent("RigidBody");
        if(rb && rb->bulletRigidBody) {
            world->dynamicsWorld->removeRigidBody(rb->bulletRigidBody);
            rb->bulletRigidBody = nullptr;
        }
    }
    else {
        IsStatic = false;
//        RigidBody *rb = (RigidBody*)GetComponent("RigidBody");
//        btTransform t;
//        t.setIdentity();
//        t.setOrigin(btVector3(transform->GetPosition().x,transform->GetPosition().y,transform->GetPosition().z));
//        btSphereShape* sphere = new btSphereShape(glm::distance(getBounds().getMin(), getBounds().getMax())/2.0);
//        btVector3 inertia(0,0,0);
//        float mass = 5.0;
//        if (mass != 0)
//            sphere->calculateLocalInertia(mass, inertia);
//        btMotionState* motion = new btDefaultMotionState(t);
//        btRigidBody::btRigidBodyConstructionInfo info(mass, motion, sphere);
//        rb->bulletRigidBody = new btRigidBody(info);
//        rb->bulletRigidBody->setActivationState(DISABLE_DEACTIVATION);
//        rb->bulletRigidBody->setFriction(1.f);
//        rb->bulletRigidBody->setRollingFriction(0.3f);
//        rb->bulletRigidBody->setAnisotropicFriction(sphere->getAnisotropicRollingFrictionDirection(),btCollisionObject::CF_ANISOTROPIC_ROLLING_FRICTION);
//        //    rigidBody->bulletRigidBody->setCollisionFlags(0); // Make it a static object
//        
//        world->dynamicsWorld->addRigidBody(rb->bulletRigidBody);
    }
}

Bounds GameObject::getBounds() {
    MeshRenderer *meshRenderer = (MeshRenderer*) this->GetComponent("MeshRenderer");
    if (meshRenderer) {
        return meshRenderer->model->bounds.TransformedBounds(this->transform);
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
        else if (name.compare("Death") == 0) {
            component = (Component*) new Death();
        }
		else if (name.compare("PathRenderer") == 0) {
			component = (Component*) new PathRenderer();
        } 
		else if (name.compare("SkyboxRenderer") == 0) {
			component = (Component*) new SkyboxRenderer();
		}
		else if (name.compare("HudRenderer") == 0) {
			component = (Component*) new HUDRenderer();
		}
		else if (name.compare("Clickable") == 0) {
			component = (Component*) new Clickable();
		}
		else if (name.compare("Button") == 0) {
			component = (Component*) new Button();
		}
		else if (name.compare("Character") == 0) {
			component = (Component *) new Character();
        }
        else if (name.compare("Light") == 0) {
            component = (Component *) new Light();
        }
        else if (name.compare("SheepDestination") == 0) {
            component = (Component *) new SheepDestination();
        } else if (name.compare("TextName") == 0) {
            component = (Component *) new TextName();
        }
		else if (name.compare("ParticleRenderer") == 0) {
			component = (Component *) new ParticleRenderer();
		}
		else if (name.compare("WaterRenderer") == 0) {
			component = (Component *) new WaterRenderer();
		}
		else {
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

GameObject::~GameObject() {
	for (auto it = components.begin(); it != components.end(); ++it) {
		delete it->second;
	}
}

Component *GameObject::GetComponent(std::string name) {
    return components[name];
}

void GameObject::RemoveComponent(std::string name) {
	delete components[name];
    components[name] = nullptr;
}

void GameObject::Destroy() {
    world->RemoveGameObject(this);
}

//template <typename Writer>
void GameObject::Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) {
    writer.StartObject();
    writer.Key("ObjectType");
    writer.String(name.c_str());
    writer.Key("Static");
    writer.Bool(IsStatic);
    writer.Key("Components");
    writer.StartObject();
    Light *light = (Light *)GetComponent("Light");
    if (light) light->Serialize(writer);
    transform->Serialize(writer);
    writer.EndObject();
    writer.EndObject();
}
