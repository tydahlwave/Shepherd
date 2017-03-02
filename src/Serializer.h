//  Serializer.h
//  SimpleGame
//
//  Created by Michael Cantrell on 3/2/17.
//
//

#ifndef Serializer_h
#define Serializer_h


#include "World.h"
#include "rapidjson/document.h"

class Serializer {
public:
    static void SerializeWorld(World *world);
    static void DeserializeWorld(World *world);
private:
    static void DeserializeSphere(rapidjson::Value &v, World *world);
    static void DeserializeBoulder(rapidjson::Value &v, World *world);
    static void DeserializeTree(rapidjson::Value &v, World *world);
    static Transform *DeserializeTransform(rapidjson::Value &v);
};

#endif /* World_h */
