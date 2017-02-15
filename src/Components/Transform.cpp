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
    
    // Below might be wonky
    RigidBody* rb = (RigidBody*)gameObject->GetComponent("RigidBody");
    if(rb && rb->bulletRigidBody) {
        btQuaternion quat = btQuaternion(glm::radians(rot.y), glm::radians(rot.x), glm::radians(rot.z)); // yaw, pitch, roll
//        if (!rb->bulletRigidBody->isKinematicObject()) {
            btTransform form = rb->bulletRigidBody->getCenterOfMassTransform();
			//btTransform formorig = form;
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
	if (rot.x != rot.x || rot.y != rot.y || rot.z != rot.z)
		return;
	rotation = rot;
}

void Transform::SetScale(glm::vec3 sc) {
    scale = sc;
    
    RigidBody* rb = (RigidBody*)gameObject->GetComponent("RigidBody");
    if(rb && rb->bulletRigidBody) {
        rb->bulletRigidBody->getCollisionShape()->setLocalScaling(btVector3(sc.x, sc.y, sc.z));
        gameObject->world->dynamicsWorld->updateSingleAabb(rb->bulletRigidBody);
    }
}
