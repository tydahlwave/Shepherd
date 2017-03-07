//
//  AnimationSystem.hpp
//  Shepherd
//
//  Created by Hermes Build Machine on 3/5/17.
//
//

#ifndef AnimationSystem_h
#define AnimationSystem_h

#include <stdio.h>
#include "World.h"
#include "Components/Animation.h"

class AnimationSystem {
public:
    AnimationSystem() {};
    virtual ~AnimationSystem() {};
    
    void Update(float deltaTime, World *world);
};
#endif /* AnimationSystem_h */
