//
//  EntityFactory.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef EntityFactory_h
#define EntityFactory_h

#include "GameObject.h"
#include "World.h"

class EntityFactory {
public:
    static GameObject *createMainCamera(World *world);
    static GameObject *createBunny(World *world);
    static GameObject *createGround(World *world);
    static GameObject *createBarrier(World *world);
    static GameObject *createCube(World *world, float width, float height, float depth, float mass);
    static GameObject *createPhysicsGround(World *world);
};

#endif /* EntityFactory_h */
