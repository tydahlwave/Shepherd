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
            std::cout<<anim->name<< "Udpating at dT : " << deltaTime <<  std::endl;
            isAnim->skeleton.Update(1);
            //UpdateSkeleton();   
        }
    }
}
