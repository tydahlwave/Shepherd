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
        if (!rb->bulletRigidBody->isKinematicObject()) {
            btTransform form = rb->bulletRigidBody->getCenterOfMassTransform();
            form.setOrigin(btVector3(pos.x, pos.y, pos.z));
            rb->bulletRigidBody->activate(true);
            rb->bulletRigidBody->setCenterOfMassTransform(form);
        } else {
            btTransform form;
            rb->bulletRigidBody->getMotionState()->getWorldTransform(form);
            form.setOrigin(btVector3(pos.x,pos.y,pos.z));
            rb->bulletRigidBody->activate(true);
            rb->bulletRigidBody->getMotionState()->setWorldTransform(form);
        }
    }
}
void Transform::SetRotation(glm::vec3 rot) {
    rotation = rot;
    
    // Below might be wonky
    RigidBody* rb = (RigidBody*)gameObject->GetComponent("RigidBody");
    if(rb && rb->bulletRigidBody) {
        if (!rb->bulletRigidBody->isKinematicObject()) {
            btTransform form = rb->bulletRigidBody->getCenterOfMassTransform();
//            form.setRotation(btQuaternion(rot.x, rot.y, rot.z))
            btQuaternion quat = btQuaternion(rot.y/180*M_PI, rot.x/180*M_PI, rot.z/180*M_PI);
            // = btQuaternion(btVector3(1, 0, 0), rot.x/180*M_PI);
//            quat.setRotation(btVector3(0, 1, 0), rot.y/180*M_PI);
//            quat.setRotation(btVector3(0, 0, 1), rot.z/180*M_PI);
            form.setRotation(quat);
            rb->bulletRigidBody->activate(true);
            rb->bulletRigidBody->setCenterOfMassTransform(form);
        } else {
            btTransform form;
            rb->bulletRigidBody->getMotionState()->getWorldTransform(form);
//            form.setRotation(btQuaternion(rot.x, rot.y, rot.z));
            btQuaternion quat = btQuaternion(rot.y/180*M_PI, rot.x/180*M_PI, rot.z/180*M_PI);
//            btQuaternion quat = btQuaternion(btVector3(1, 0, 0), rot.x/180*M_PI);
//            quat.setRotation(btVector3(0, 1, 0), rot.y/180*M_PI);
//            quat.setRotation(btVector3(0, 0, 1), rot.z/180*M_PI);
            form.setRotation(quat);
            rb->bulletRigidBody->activate(true);
            rb->bulletRigidBody->getMotionState()->setWorldTransform(form);
        }
    }
}
void Transform::SetScale(glm::vec3 sc) {
    scale = sc;
}
