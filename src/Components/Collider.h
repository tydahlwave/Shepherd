//
//  Collider.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Collider_h
#define Collider_h

#include "Component.h"
//#include "RigidBody.h"

class Collider : public Component {
protected:
    Collider() { name = "Collider"; };
    virtual ~Collider() {};
    
//    RigidBody *rigidBody; // The rigidbody of the gameobject the component is attached to
    void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) {
        
    }
};

#endif /* Collider_h */
