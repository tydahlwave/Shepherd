//
//  RigidBody.cpp
//  Shepherd
//
//  Created by Michael Cantrell on 1/27/17.
//
//

#include "Components/RigidBody.h"
#include "GameObject.h"

RigidBody::RigidBody() {
    name = "RigidBody";
    if(bulletRigidBody)
        gameObject->world->dynamicsWorld->addRigidBody(bulletRigidBody);
}

RigidBody::RigidBody(glm::vec3 v, glm::vec3 a, bool useG) : RigidBody() {
    velocity = v;
    acceleration = a;
    useGravity = useG;
}

