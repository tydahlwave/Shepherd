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
#include "Bounds.h"
#include "rapidjson/prettywriter.h"

class GameObject : public Object {
public:
    GameObject();
    GameObject(std::string name);
    GameObject(std::string name, std::vector<std::string> componentNames);
    virtual ~GameObject() {};
    
    World *world = nullptr;
    Transform *transform = nullptr;
    
    Bounds getBounds();
    
    Component *AddComponent(std::string name);
    Component *GetComponent(std::string name);
    void RemoveComponent(std::string name);
    
    // Safely remove game object from game
    void Destroy();
    
    //template <typename Writer> // i changed the type from Writer to PrettyWriter to get it to compile
    void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer);
private:
    std::map<std::string, Component*> components;
};

#endif /* GameObject_h */
