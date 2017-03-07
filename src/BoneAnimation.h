//
//  BoneAnimation.h
//  Shepherd
//
//  Created by Hermes Build Machine on 3/6/17.
//
//

#ifndef BoneAnimation_h
#define BoneAnimation_h

//
//  Animation.hpp
//  Shepherd
//
#include <stdio.h>
#include <glm/glm.hpp>
#include <string>


//Here are two convenience functions, they will become very convenient later on.
//IMPORTANT!!!! The 0.041666666667 float in these functions is equal to
//    1.0 / 24.0, which is how many frames there are in a second, as the software
//    I am using (Blender 3D), plays animations at 24 frames per second.

//This first function converts from a time passed in (in seconds) to the number
//of frames to which it corresponds.
static float TimeToFrame(float time)
{
    float frame = 0.041666666667;
    return time / frame;
}

//This next function converts from a start frame and an end frame passed in to
//a start time and an end time, both of which are stored in a glm::vec2.
static glm::vec2 FramesToTime(glm::vec2 frames)
{
    float frame = 0.041666666667;
    return frames * frame;
}

class BoneAnimation {
public:
    
    std::string name;
    float start_time; //The start time of the animation (in seconds)
    float end_time;   //The end time of the animation (in seconds)
    int priority;   //The priority of this animation, if another
    //animation is being played and we try to play
    //this one, this one will only play if it has
    //a lower priority than the one being played,
    //this way, the more important animations are
    //always the ones selected to be played.
    
    BoneAnimation()
    {
        start_time = end_time = priority = 0; //Set everything to zero...
    }
    
    BoneAnimation(std::string in_name, glm::vec2 times, int in_priority)
    {
        name = in_name;
        start_time = times.x;
        end_time = times.y;
        priority = in_priority;
    }
    
};






#endif /* BoneAnimation_h */
