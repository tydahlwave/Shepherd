//
//  RigidBody.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef RigidBody_h
#define RigidBody_h

#include "Component.h"

class RigidBody : public Component {
public:
    RigidBody() { name = "RigidBody"; };
    RigidBody(glm::vec3 v, glm::vec3 a, bool useG) :RigidBody() { velocity = v; acceleration = a; useGravity = useG; }
    virtual ~RigidBody() {};
    
    glm::vec3 velocity = glm::vec3(0, 0, 0);
    glm::vec3 acceleration = glm::vec3(0, 0, 0);
    bool useGravity = false;
    bool isKinematic = false; // Kinematic objects are not affected by forces/physics
};

#endif /* RigidBody_h */
