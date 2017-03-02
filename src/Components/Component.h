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

#include "../Object.h"
#include "../rapidjson/prettywriter.h"

class GameObject;

class Component : public Object {
public:
    Component() { name = "Component"; };
    Component(GameObject* go) :Component() { gameObject = go; };
    virtual ~Component() {};
    
    GameObject *gameObject = nullptr;
    
    // Note: this cannot be made pure virtual because Components are allocated... to make pure virtual, say "=0" instead of "{}"
    void virtual Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) { puts("If this prints, that means the function wasn't overridden properly." ); };
};

#endif /* Component_h */
