//
//  AnimationSystem.cpp
//  Shepherd
//
//  Created by Hermes Build Machine on 3/5/17.
//
//

#include "AnimationSystem.h"

void AnimationSystem::Update(float deltaTime, World *world)
{
    for(GameObject* anim: world->GetGameObjects())
    {
        Animation* isAnim = (Animation*) anim->GetComponent("Animation");
        if(isAnim)    //If the object is rigged...
        {
            isAnim->skeleton.Update(deltaTime);
            //UpdateSkeleton();    //... we update the skeleton (remember that this
            //function is virtual, it will always call the
            //version furthest from the base version)
        }
    }
}
