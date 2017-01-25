//
//  Physics.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Physics_h
#define Physics_h

#include "World.h"
#include "GameObject.h"
#include "Bounds.h"

struct Collision {
    Collision(GameObject *g1, GameObject *g2, Bounds *i) :gameObject1(g1), gameObject2(g2), intersection(i) {};
    GameObject *gameObject1;
    GameObject *gameObject2;
    Bounds *intersection;
};

class Physics {
public:
    Physics() {};
    virtual ~Physics() {};
    
    glm::vec3 gravity = glm::vec3(0, -100, 0);
    int bunniesCollected = 0;
    
    void Update(float deltaTime, World &world);
    Bounds *BoundsForGameObject(GameObject *gameObject);
private:
    void ComputeCollisions(World &world);
    void ResolveCollisions(std::vector<Collision> collisions);
//    Bounds *BoundsForGameObject(GameObject *gameObject);
};

#endif /* Physics_h */
