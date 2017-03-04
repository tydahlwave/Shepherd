//  Serializer.h
//  SimpleGame
//
//  Created by Michael Cantrell on 3/2/17.
//
//

#ifndef Serializer_h
#define Serializer_h


#include "World.h"
#include "Components/Light.h"
#include "rapidjson/document.h"

class Serializer {
public:
    static void SerializeWorld(World *world);
    static void DeserializeWorld(World *world);
private:
    static GameObject *DeserializeSphere(rapidjson::Value &v, World *world);
    static GameObject *DeserializeBoulder(rapidjson::Value &v, World *world);
    static GameObject *DeserializeTree(rapidjson::Value &v, World *world);
    static GameObject *DeserializeLightObject(rapidjson::Value &v, World *world);
    static Transform *DeserializeTransform(rapidjson::Value &v);
    static Light *DeserializeLightComponent(rapidjson::Value &v);
};

#endif /* World_h */
