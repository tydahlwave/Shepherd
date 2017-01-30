//
//  Transform.cpp
//  Shepherd
//
//  Created by Michael Cantrell on 1/29/17.
//
//

#include "Transform.h"
#include "../GameObject.h"
#include "RigidBody.h"


void Transform::SetPosition(glm::vec3 pos) {
    position = pos;
    RigidBody* rb = (RigidBody*)gameObject->GetComponent("RigidBody");
    if(rb && rb->bulletRigidBody) {
        btTransform *form = new btTransform();
        rb->bulletRigidBody->getMotionState()->getWorldTransform(*form);
        form->setOrigin(btVector3(pos.x,pos.y,pos.z));
    }
}
void Transform::SetRotation(glm::vec3 rot) {
    rotation = rot;
    
    // Below might be wonky
    RigidBody* rb = (RigidBody*)gameObject->GetComponent("RigidBody");
    if(rb && rb->bulletRigidBody) {
        btTransform *form = new btTransform();
        rb->bulletRigidBody->getMotionState()->getWorldTransform(*form);
        form->setRotation(btQuaternion(rot.x, rot.y, rot.z));
    }
}
void Transform::SetScale(glm::vec3 sc) {
    scale = sc;
}
