//
//  GameObject.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef GameObject_h
#define GameObject_h

#include <string>
#include <vector>
#include <map>

#include "Object.h"
#include "World.h"
#include "Components/Component.h"
#include "Components/Transform.h"

class World;
class Component;
class Transform;

class GameObject : public Object {
public:
    GameObject();
    GameObject(std::string name);
    GameObject(std::string name, std::vector<std::string> componentNames);
    virtual ~GameObject() {};
    
    World *world = nullptr;
    Transform *transform = nullptr;
    
    Component *AddComponent(std::string name);
    Component *GetComponent(std::string name);
private:
    std::map<std::string, Component*> components;
};

#endif /* GameObject_h */
