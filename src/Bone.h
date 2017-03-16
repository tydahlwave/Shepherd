//
//  Bone.hpp
//  Shepherd
//
//  Created by Hermes Build Machine on 3/5/17.
//
//

#ifndef Bone_h
#define Bone_h

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/transform.hpp>

#include "BoneAnimation.h"

#include <string>
#include <vector>

#include <assimp/scene.h>
#include <iostream>
//#include "Components/Animation.h"


class Mesh;
class BoneAnimation;
class GameObject;
class Skeleton;


//may not be a-ok *********
static aiMatrix4x4 GLMMat4ToAi(glm::mat4 mat)
{
    return aiMatrix4x4(mat[0][0],mat[0][1],mat[0][2],mat[0][3],
                       mat[1][0],mat[1][1],mat[1][2],mat[1][3],
                       mat[2][0],mat[2][1],mat[2][2],mat[2][3],
                       mat[3][0],mat[3][1],mat[3][2],mat[3][3]);
//    return aiMatrix4x4(mat[0][0],mat[1][0],mat[2][0],mat[3][0],
//                       mat[0][1],mat[1][1],mat[2][1],mat[3][1],
//                       mat[0][2],mat[1][2],mat[2][2],mat[3][2],
//                       mat[0][3],mat[1][3],mat[2][3],mat[3][3]);
}


//ian's works! keeping old for reference
static glm::mat4 AiToGLMMat4(aiMatrix4x4& ai)
{
//    glm::mat4 tmp;
//    tmp[0][0] = in_mat.a1;
//    tmp[1][0] = in_mat.b1;
//    tmp[2][0] = in_mat.c1;
//    tmp[3][0] = in_mat.d1;
//    
//    tmp[0][1] = in_mat.a2;
//    tmp[1][1] = in_mat.b2;
//    tmp[2][1] = in_mat.c2;
//    tmp[3][1] = in_mat.d2;
//    
//    tmp[0][2] = in_mat.a3;
//    tmp[1][2] = in_mat.b3;
//    tmp[2][2] = in_mat.c3;
//    tmp[3][2] = in_mat.d3;
//    
//    tmp[0][3] = in_mat.a4;
//    tmp[1][3] = in_mat.b4;
//    tmp[2][3] = in_mat.c4;
//    tmp[3][3] = in_mat.d4;
//    return tmp;
    return glm::mat4(
                     ai.a1, ai.b1, ai.c1, ai.d1,
                     ai.a2, ai.b2, ai.c2, ai.d2,
                     ai.a3, ai.b3, ai.c3, ai.d3,
                     ai.a4, ai.b4, ai.c4, ai.d4
                     );
    
}

static void printGLMMat4(glm::mat4  mat)
{
//    std::cout<<mat[0][0]<<"  "<<mat[0][1]<<"  "<<mat[0][2]<<"  "<<mat[0][3]<<"  "<<std::endl;
//    std::cout<<mat[1][0]<<"  "<<mat[1][1]<<"  "<<mat[1][2]<<"  "<<mat[1][3]<<"  "<<std::endl;
//    std::cout<<mat[2][0]<<"  "<<mat[2][1]<<"  "<<mat[2][2]<<"  "<<mat[2][3]<<"  "<<std::endl;
//    std::cout<<mat[3][0]<<"  "<<mat[3][1]<<"  "<<mat[3][2]<<"  "<<mat[3][3]<<"  "<<std::endl;
    
    //column major
    std::cout<<mat[0][0]<<"  "<<mat[1][0]<<"  "<<mat[2][0]<<"  "<<mat[3][0]<<"  "<<std::endl;
    std::cout<<mat[0][1]<<"  "<<mat[1][1]<<"  "<<mat[2][1]<<"  "<<mat[3][1]<<"  "<<std::endl;
    std::cout<<mat[0][2]<<"  "<<mat[1][2]<<"  "<<mat[2][2]<<"  "<<mat[3][2]<<"  "<<std::endl;
    std::cout<<mat[0][3]<<"  "<<mat[1][3]<<"  "<<mat[2][3]<<"  "<<mat[3][3]<<"  "<<std::endl;
    
}

//in house position struct
struct PositionKey
{
    glm::vec3 position;
    float startTime;
    
    PositionKey(const glm::vec3 pos, float start){
        position = pos;
        startTime = start;
    }
    PositionKey(){}
};

//in house rotation struct
struct RotationKey
{
    glm::quat rotation;
    float startTime;
    
    RotationKey(const glm::quat rot, float start){
        rotation = rot;
        startTime = start;
    }
    
    RotationKey()
    {
        
    }
};

//in house animation node
struct AnimationNode
{
    std::string nodeName;
    std::vector<PositionKey> positionKeys;
    std::vector<RotationKey> rotationKeys;
    //leaving room for scaling
    
    AnimationNode(std::string node, std::vector<PositionKey> position, std::vector<RotationKey> rotation){
        nodeName = node;
        positionKeys = position;
        rotationKeys = rotation;
    }
    
};



//Bone class defining bone behavior stripped from assimp
class Bone
{
public:
    std::string name;    //The bone's name as loaded by ASSIMP.
    
    Mesh* mesh;    //The mesh this bone is going to affect.
    
    aiNode* node;    //This bone's corresponding aiNode
    
    aiNodeAnim* animNode;    //This is the bone's corresponding aiNodeAnim,
    
    AnimationNode* actualAnimNode;
    
    Bone* parent_bone;    //A pointer to this bone's parent bone.
    
    glm::mat4 parent_transforms;    //all transforms to the root
    
    glm::mat4 offset_matrix;    //bone local space
    
    Skeleton* parent_skeleton;  //The parent skeleton of this bone.
    
    //Keyframe Data
    glm::vec3 pos;
    glm::quat rot;
    glm::vec3 scale;
    glm::vec3 p1;
    glm::vec3 p2;
 
    int id;
    
    Bone(){name = ""; id = -2;}
    
    unsigned int FindPosition(float time);
    glm::vec3 CalcInterpolatedPosition(float time);
    unsigned int FindRotation(float time);
    glm::quat CalcInterpolatedRotation(float time);
    
    Bone(Mesh* in_mesh, unsigned int in_id, std::string in_name, aiMatrix4x4 in_o_mat);
    Bone(Mesh* in_mesh, unsigned int in_id, std::string in_name, glm::mat4 in_o_mat);
    
    void UpdateKeyframeTransform(float time);
    glm::mat4 GetParentTransforms();
};


//Skeleton holds the bones for a class
class Skeleton
{
public:
    std::vector<Bone> bones;
    glm::mat4 globalInverseTransform;
    std::vector<glm::mat4> boneMats;
    float time;
    
    float start_time;
    float end_time;
    
    BoneAnimation* active_animation;
    BoneAnimation* idle_animation;
    
    bool anim_play;
    bool anim_loop;
    
    Skeleton()
    {
        time = start_time = end_time = 0;
        active_animation = nullptr;
        
        anim_loop = false;
    }
    
    Skeleton(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform)
    {
        Init(in_bones,in_globalInverseTransform);
    }
    
    //initialize bone state
    void Init(std::vector<Bone> in_bones, glm::mat4 in_globalInverseTransform)
    {
        bones = in_bones;
        globalInverseTransform = in_globalInverseTransform;
        
        time = start_time = end_time = 0;
        active_animation = nullptr;
        idle_animation = nullptr;
        
        anim_loop = false;
        
        for(int i = 0; i < bones.size(); i++)
        {
            std::cout<<bones.at(i).name<<std::endl;
            bones.at(i).parent_skeleton = this;
        }
    }
    
    //This next function is pretty self-explanatory...
    Bone* FindBone(std::string name)
    {
        for(int i = 0; i < bones.size(); i++)
        {
            if(bones.at(i).name == name)
                return &bones.at(i);
        }
        return nullptr;
    }
    
    
    //Play selected bone animation
    void PlayAnimation(BoneAnimation& anim, bool loop, bool reset_to_start)
    {
        //If there's an animation currently playing
        if(active_animation != nullptr)
        {
            //And this animation is more important than the current one
            if(anim.priority < active_animation->priority)
                //Set the current animation to the one passed in.
                active_animation = &anim;
            else
                //Do nothing.
                return;
        }
        else
            //Set the current animation to the one passed in.
            active_animation = &anim;
        
        start_time = active_animation->start_time;
        end_time = active_animation->end_time;
        
        anim_play = true;
        anim_loop = loop;
        
        //The reset_to_start variable determines whether or not the animation
        //should restart upon playing.
        if(reset_to_start)
            time = active_animation->start_time;
    }
    
    //This function stops animating
    void StopAnimating()
    {
        time = end_time;
        active_animation = nullptr;
        anim_play = false;
    }
    
    //This function sets the idle animation
    void SetIdleAnimation(BoneAnimation* in_anim)
    {
        idle_animation = in_anim;
    }
    
    //Gather all of the bones' global transforms
    void UpdateBoneMatsVector()
    {
        
        if(bones.size() == 0)
        {
            std::cout<<"WHY IS BONE SIZE 0 "<<std::endl;
            return;
        }
        
        //Make sure there's nothning left in the vector.
        boneMats.clear();

        //update all the mats for the bones
        for(int i = 0; i < 100; i++)
        {
            //If we are past the number of bones in the actual skeleton, we simply
            //pass in an identity matrix.
            if(i > bones.size() - 1)
                boneMats.push_back(glm::mat4(1.0));
            else
            {
                //To get the concatenated transformation, we multiply the concatenated transforms of
                //the bone's parents with the bone's transformation.
                
                
                glm::mat4 concatenated_transformation = (bones.at(i).GetParentTransforms() * AiToGLMMat4(bones.at(i).node->mTransformation));
                
                
                /*std::cout<<"Bone "<<bones[i].name<<"   Parent transform"<<std::endl;
                printGLMMat4(bones.at(i).GetParentTransforms());*/
                //glm::mat4 concatenated_transformation = AiToGLMMat4(bones.at(i).node->mTransformation);
                
//                std::cout<<"Bone "<<bones[i].name<<"   Cat transform"<<std::endl;
//                printGLMMat4(concatenated_transformation);
//                
//                std::cout<<"Bone "<<bones[i].name<<"   node transformation"<<std::endl;
//                printGLMMat4(AiToGLMMat4(bones.at(i).node->mTransformation));
//                                                                     
//
                glm::mat4 temp = globalInverseTransform * concatenated_transformation* bones.at(i).offset_matrix;
                std::cout<<"Bone "<<bones[i].name<<"   final transform"<<std::endl;
                printGLMMat4(temp);
                //std::cout<<"Bone "<<bones[i].name<<"   FULL CALC transform"<<std::endl;
                //printGLMMat4(globalInverseTransform * AiToGLMMat4(bones.at(i).node->mTransformation) * bones.at(i).offset_matrix);
//                std::cout<<"Bone "<<bones[i].name<<"   OFFSET MATRIX transform"<<std::endl;
//                printGLMMat4(bones.at(i).offset_matrix);
                
                
                boneMats.push_back(globalInverseTransform * concatenated_transformation * bones.at(i).offset_matrix);
            }
        }
    }
                                                                     
    void Update(float deltaTime)
    {
       //std::cout<<"Update Skeleton at dT"<< deltaTime <<std::endl;
       UpdateBoneMatsVector();
        if(!anim_play)
            //If we're not playing an animation, do nothing.
            return;
        
        //Update the time variable by adding the delta time of the last frame
        //It's * 0.001f because the delta time is in milliseconds, and we
        //need it in seconds.
        time += deltaTime * 0.001f;
        
        //Make sure the time can't be less than our animation's start time.
        if(time < start_time)
            time = start_time;
        
        //Make sure the time can't be greater than our animation's end time.
        if(time > end_time)
        {
            if(anim_loop)
                //If looping is set, then loop!
                time = start_time;
            else
                //Else, give up.
                StopAnimating();
        }
        std::cout<<"got to Update w time of "<< time <<std::endl;
        
        for(int i = 0; i < bones.size(); i++)
        {
            std::cout<<"Updating ky trans of bone: "<< bones[i].name <<std::endl;
            bones.at(i).UpdateKeyframeTransform(time);
            //bones.at(i).UpdateKeyframeTransform(1);
        }
    }
};

#include <stdio.h>
#endif /* Bone_h */
