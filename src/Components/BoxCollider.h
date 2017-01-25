//
//  BoxCollider.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#ifndef BoxCollider_h
#define BoxCollider_h

#include "Collider.h"

class BoxCollider : public Collider {
public:
    BoxCollider() { name = "BoxCollider"; };
    BoxCollider(glm::vec3 c, glm::vec3 s) :BoxCollider() { center = c; size = s; }
    virtual ~BoxCollider() {};
    
    glm::vec3 center = vec3(0, 0, 0);
    glm::vec3 size = vec3(0, 0, 0);
};

#endif /* BoxCollider_h */
