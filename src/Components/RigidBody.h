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
#include "../Time.h"
#ifdef WIN32
#include <btBulletCollisionCommon.h>
#else
#include <BulletDynamics/btBulletCollisionCommon.h>
#endif

class RigidBody : public Component {
public:
    RigidBody();
	RigidBody(glm::vec3 v, glm::vec3 a, bool useG);
    virtual ~RigidBody() {};
    
    glm::vec3 velocity = glm::vec3(0, 0, 0);
    glm::vec3 acceleration = glm::vec3(0, 0, 0);
    bool useGravity = false;
    bool isKinematic = false; // Kinematic objects are not affected by forces/physics
	bool isPushPullable = true;
    btRigidBody *bulletRigidBody = nullptr;
    
    long pointInTime = 0;
    int waitTime = 0;
    
    void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) {
        
    }
};

#endif /* RigidBody_h */
