//
//  Animation.cpp
//  Shepherd
//
//  Created by Hermes Build Machine on 3/5/17.
//
//

#include <stdio.h>
#include "Animation.h"


void Animation::AddAnimation(BoneAnimation& in_anim)
{
    animations.push_back(in_anim);
}

BoneAnimation* Animation::FindAnimation(std::string anim_to_find)
{
    for(int i = 0; i < animations.size(); i++)
    {
        if(animations.at(i).name == anim_to_find)
            return &animations.at(i);
    }
    return nullptr;
}

void Animation::PlayAnimation(BoneAnimation& anim, bool loop, bool reset_to_start)
{
    skeleton.PlayAnimation(anim,loop,reset_to_start);
}

void Animation::StopAnimating()
{
    skeleton.StopAnimating();
}
