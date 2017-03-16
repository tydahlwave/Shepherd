//
//  Bone.cpp
//  Shepherd
//
//  Created by Hermes Build Machine on 3/5/17.
//
//

#include "Bone.h"

Bone::Bone(Mesh* in_mesh, unsigned int in_id, std::string in_name, aiMatrix4x4 in_o_mat)
{
    id = in_id;
    name = in_name;
    offset_matrix = AiToGLMMat4(in_o_mat);
    
    mesh = in_mesh;
    
    parent_bone = nullptr;
    node = nullptr;
}


Bone::Bone(Mesh* in_mesh, unsigned int in_id, std::string in_name, glm::mat4 in_o_mat)
{
    id = in_id;
    name = in_name;
    offset_matrix = in_o_mat;
    
    mesh = in_mesh;
    
    parent_bone = nullptr;
    node = nullptr;
}

//This function simply concatenates the parents' transforms.
//This is vital for calculating this bone's final transformation.
glm::mat4 Bone::GetParentTransforms()
{
    Bone* b = parent_bone;    //In order to recursively concatenate the transforms,
    //we first start with this bone's parent.
    std::vector<glm::mat4> mats;    //Where we'll store the concatenated transforms.
    
    
    
    
    while(b != nullptr)    //As long as 'b' has a parent (see the end of the loop
    {                      //to avoid confusion).
        glm::mat4 tmp_mat = AiToGLMMat4(b->node->mTransformation); //This bone's transformation.
        mats.push_back(tmp_mat);
        
        //std::cout<<"Bone "<<b->node->mName.C_Str()<<"   Parent transform   "<<std::endl;
        //printGLMMat4(tmp_mat);
        
        b = b->parent_bone;    //We set b to its own parent so the loop can continue.
    }
    
    glm::mat4 concatenated_transforms;
    //IMPORTANT!!!! This next loop must be done in reverse,
    //as multiplication with matrices is not commutative.
    for(int i = mats.size()-1; i >= 0; i--)
        //concatenated_transforms = mats.at(i) * concatenated_transforms;
        concatenated_transforms *= mats.at(i);
    return concatenated_transforms;    //Finally, we return the concatenated transforms.*/
//    if(b != nullptr)
//    {
//        return AiToGLMMat4(parent_bone->node->mTransformation);
//    }

}


//All of these Find functions return an integer because they aren't returning
//keyframes themselves, they are returning indices to the current keyframe
//within our stored array of keyframes, animNode.

unsigned int Bone::FindPosition(float time)
{
    //std::cout<<"Got here F 1"<<std::endl;
    //std::cout<<"NUM KEYS    " << animNode->mNumPositionKeys<<std::endl;
    if(animNode->mNumPositionKeys > 0)
    {
    for(unsigned int i = 0 ; i < animNode->mNumPositionKeys - 1 ; i++)
    {
        
        //std::cout<< i <<"   MTIME    " << actualAnimNode->positionKeys[i].startTime<<std::endl;

        //If the time passed in is less than the time of the next
        //keyframe, then this is the keyframe we want!
        //std::cout<<"next frame: " << (float)animNode->mPositionKeys[i + 1].mTime<<"    : as double " << animNode->mPositionKeys[i + 1].mTime <<std::endl;
        if(time < (float)(actualAnimNode->positionKeys[i + 1].startTime))
        {
            //std::cout<<"  RETURNING   " << i << std::endl;
            return i;
        }
    }
    }
    return 0;    //If we don't find anything, just return 0.
}

unsigned int Bone::FindRotation(float time)
{
    for(unsigned int i = 0 ; i < animNode->mNumRotationKeys - 1 ; i++)
    {
        //Same as with the position.
        if(time < (float)actualAnimNode->rotationKeys[i+1].startTime)
        {
            return i;
        }
    }
    return 0;
}


glm::vec3 Bone::CalcInterpolatedPosition(float time)
{
    //If there's only one keyframe for the position, we return it.
    //This avoids any possible out of range errors later on in the function
    //as we're dealing with an array.
    if(animNode->mNumPositionKeys == 1)
    {
        glm::vec3 val = actualAnimNode->positionKeys[0].position;
        return val;
    }
    //std::cout<<"Got here 1  Time = "<< time << std::endl;
    //std::cout<<"Got here 1  node = "<< animNode->mNodeName.data<< std::endl;
    
    
    //The index of our current position, as well as the index that follows.
    //This will allow us to interpolate between the two values.
    unsigned int PositionIndex = FindPosition(time);
    unsigned int NextPositionIndex = (PositionIndex + 1);
    //std::cout<<"Got here 2: Position nDx: "<< PositionIndex << std::endl;
    
    //DeltaTime is the amount of time between the two keyframes.
    float DeltaTime = actualAnimNode->positionKeys[NextPositionIndex].startTime - actualAnimNode->positionKeys[PositionIndex].startTime;
    //The factor is simply how much time has passed since the last keyframe,
    //divided by the amount of time between the two keyframes (the DeltaTime)
    //to get the percentage, or how far along between the two keyframes we are.
    float Factor = (time - (float)actualAnimNode->positionKeys[PositionIndex].startTime) / DeltaTime;
    //std::cout<<"Got here 3"<<std::endl;
    
    //The start and end positions (the position values of each of the keyframes)
    //std::cout<<"Got here 4"<<std::endl;
    
    //Here we convert them to glm matrices...
    glm::vec3 p1 = actualAnimNode->positionKeys[PositionIndex].position;
    glm::vec3 p2= actualAnimNode->positionKeys[NextPositionIndex].position;
    
    //std::cout<<"Got here 5"<<std::endl;
    
    
    //and here we linearly interpolate between the two keyframes.
    glm::vec3 val = glm::mix(p1,p2,Factor);
    
    return val;
}

//This function is identical to the position one, except it's for rotation ...
//believe it or not.
glm::quat Bone::CalcInterpolatedRotation(float time)
{
    if(animNode->mNumRotationKeys == 1)
    {
        aiQuaternion assimp_val = animNode->mRotationKeys[0].mValue;
        glm::quat val = actualAnimNode->rotationKeys[0].rotation;
        return val;
    }
    
    unsigned int RotationIndex = FindRotation(time);
    unsigned int NextRotationIndex = (RotationIndex + 1);
    
    float DeltaTime = actualAnimNode->rotationKeys[NextRotationIndex].startTime - actualAnimNode->rotationKeys[RotationIndex].startTime;
    float Factor = (time - (float)actualAnimNode->rotationKeys[RotationIndex].startTime) / DeltaTime;
    
    glm::quat r1 = actualAnimNode->rotationKeys[RotationIndex].rotation;
    glm::quat r2= actualAnimNode->rotationKeys[NextRotationIndex].rotation;
    
    glm::quat val = glm::slerp(r1,r2,Factor);
    return val;
}

//This function will update our node's transformation to match that of
//our keyframes.
void Bone::UpdateKeyframeTransform(float time)
{
    if(animNode == nullptr)
        return;
    //std::cout<<name<<"  entered update key trans, bone animNode != null   :  " << animNode->mNodeName.data << "   : pos and rot keys:  " << animNode->mNumPositionKeys << animNode->mNumRotationKeys << std::endl;
    pos = CalcInterpolatedPosition(time);

//    if(!std::strcmp(animNode->mNodeName.data , "Bip01_Pelvis"))
//    {
//        std::cout << animNode->mNodeName.data << "   is  " << std::strcmp(animNode->mNodeName.data , "Bip01_Pelvis") << std::endl;
//        rot = glm::quat(.5,0,1,0);
//    }
    rot = CalcInterpolatedRotation(time);

    //pos = glm::vec3(1);
    scale = glm::vec3(1);
    //std::cout<<name<<"  interpolated position is:  " << pos.x << " : " << pos.y << " : " << pos.z << std::endl;
    //std::cout<<name<<"  interpolated rotation is:  " << rot.x << " : " << rot.y << " : " << rot.z << std::endl;
    
    glm::mat4 mat;
    //mat = glm::translate(pos) * glm::mat4_cast(rot);
    mat *= glm::translate(pos);
    mat *= glm::mat4_cast(rot);
    //mat *= glm::rotate(rot)
    //mat *= glm::scale(scale);
    
    //printGLMMat4(mat);
    
    node->mTransformation = GLMMat4ToAi(glm::transpose(mat));
}
