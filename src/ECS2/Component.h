//
//  Component.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Component_h
#define Component_h

#include "glm/glm.hpp"

#include "Object.h"
#include "GameObject.h"

class GameObject;

class Component : public Object {
public:
    Component() { name = "Component"; };
    Component(GameObject* go) :Component() { gameObject = go; };
    virtual ~Component() {};
    
    GameObject *gameObject = nullptr;
};

#endif /* Component_h */
