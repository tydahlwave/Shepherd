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
    //return;
    
    // Below might be wonky
    RigidBody* rb = (RigidBody*)gameObject->GetComponent("RigidBody");
    if(rb && rb->bulletRigidBody) {
        btQuaternion quat = btQuaternion(rot.y/180*M_PI, rot.x/180*M_PI, rot.z/180*M_PI); // yaw, pitch, roll
//        if (!rb->bulletRigidBody->isKinematicObject()) {
            btTransform form = rb->bulletRigidBody->getCenterOfMassTransform();
            form.setRotation(quat);
            rb->bulletRigidBody->activate(true);
            rb->bulletRigidBody->setCenterOfMassTransform(form);
//        } else {
//            btTransform form;
//            rb->bulletRigidBody->getMotionState()->getWorldTransform(form);
//            form.setRotation(quat);
//            rb->bulletRigidBody->activate(true);
//            rb->bulletRigidBody->getMotionState()->setWorldTransform(form);
//        }
    }
}

void Transform::SetScale(glm::vec3 sc) {
    scale = sc;
    
    RigidBody* rb = (RigidBody*)gameObject->GetComponent("RigidBody");
    if(rb && rb->bulletRigidBody) {
        rb->bulletRigidBody->getCollisionShape()->setLocalScaling(btVector3(sc.x, sc.y, sc.z));
        gameObject->world->dynamicsWorld->updateSingleAabb(rb->bulletRigidBody);
    }
}

