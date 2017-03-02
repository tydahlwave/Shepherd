//
//  Serializer.h
//  Shepherd
//
//  Created by Michael Cantrell on 3/2/17.
//
//

#ifndef Serializer_h
#define Serializer_h

#include <stdio.h>

#endif /* Serializer_hpp */


//
//  World.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef World_h
#define World_h

#include <vector>

class GameObject;
class KDTree;

#include "GameObject.h"
#include "EntityFactory.h"

#include "rapidjson/prettywriter.h"
#include "rapidjson/document.h"
#include "rapidjson/writer.h"
#include "rapidjson/stringbuffer.h"
#include <fstream>
#include <sstream>

class Serializer {
public:
    void ClearGameObjects();
    static void SerializeWorld(World *world);
    static void DeserializeWorld(World *world);
private:

    static Transform *DeserializeTransform(rapidjson::Value &v);
};

#endif /* World_h */
