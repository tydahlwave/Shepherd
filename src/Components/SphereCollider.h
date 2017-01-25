//
//  SphereCollider.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#ifndef SphereCollider_h
#define SphereCollider_h

#include "Collider.h"

class SphereCollider : public Collider {
public:
    SphereCollider() { name = "SphereCollider"; };
    SphereCollider(glm::vec3 c, float r) :SphereCollider() { center = c; radius = r; }
    virtual ~SphereCollider() {};
    
    glm::vec3 center = vec3(0, 0, 0);
    float radius = 0;
};

#endif /* SphereCollider_h */
