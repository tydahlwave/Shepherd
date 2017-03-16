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
            //std::cout<<anim->name<< "Udpating at dT : " << deltaTime <<  std::endl;
            isAnim->skeleton.Update(deltaTime * 50);
            //UpdateSkeleton();   
        }
        
        
        if(anim->name.compare("Ring") == 0)
        {
            if(anim->transform->GetScale().x < 30)
            {
                glm::vec3 scale = anim->transform->GetScale();
                scale = glm::vec3(scale.x + .5f, scale.y, scale.z + .5f);
                //scale *= 2;
                anim->transform->SetScale(scale);
                //anim->transform->SetPosition(world->mainCharacter->transform->GetPosition());
            }
            else{
               anim->Destroy();
            }
        }
    }
}
