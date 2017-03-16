//
//  Animation.hpp
//  Shepherd
//
//  Created by Hermes Build Machine on 3/5/17.
//
//

#ifndef Animation_h
#define Animation_h

#include <stdio.h>
#include "../Bone.h"
#include "Component.h"

class Animation : public Component {
public:
    bool anim = false;    //This bool is here so that we can easily tell if an object//is animated/rigged.
    Skeleton skeleton;
    std::vector<BoneAnimation> animations;
    
    virtual void UpdateSkeleton(){skeleton.Update(0);}    //This function calls skeleton.Update(), but
    //I made it virtual so that you can give
    //any inheriting entities you create
    //separate behavior. It's up to you, really.
    
    Animation() { name = "Animation"; };
    virtual ~Animation() {};
    
    void AddAnimation(BoneAnimation& in_anim);
    BoneAnimation* FindAnimation(std::string anim_to_find);
    void PlayAnimation(BoneAnimation& anim, bool loop = false, bool reset_to_start = false);
    void StopAnimating();
    
};




#endif /* Animation_h */
