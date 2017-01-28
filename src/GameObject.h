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

class World;
class Transform;
class Component;

#include "World.h"
#include "Components/Transform.h"

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
