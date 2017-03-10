//
//  Bone.cpp
//  Shepherd
//
//  Created by Hermes Build Machine on 3/5/17.
//
//

#include "Bone.h"
//This here is a simple constructor, which merely sets the member data
//to that which was passed in. Fancy, I know.
Bone::Bone(Mesh* in_mesh, unsigned int in_id, std::string in_name, aiMatrix4x4 in_o_mat)
{
    id = in_id;
    name = in_name;
    offset_matrix = AiToGLMMat4(in_o_mat);
    
    mesh = in_mesh;
    
    parent_bone = nullptr;
    node = nullptr;
}
//This constructor is similar to the last one in the sense that it is
//virtually identical and whose existence seems almost pointless. Almost.
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
        concatenated_transforms = mats.at(i) * concatenated_transforms;
        //concatenated_transforms *= mats.at(i);
    return concatenated_transforms;    //Finally, we return the concatenated transforms.*/
//    if(b != nullptr)
//    {
//        return AiToGLMMat4(b->node->mTransformation);
//    }
//    else
//        return glm::mat4(1);
}


//All of these Find functions return an integer because they aren't returning
//keyframes themselves, they are returning indices to the current keyframe
//within our stored array of keyframes, animNode.

unsigned int Bone::FindPosition(float time)
{
    std::cout<<"Got here F 1"<<std::endl;
    for(unsigned int i = 0 ; i < animNode->mNumPositionKeys - 1 ; i++)
    {
        //If the time passed in is less than the time of the next
        //keyframe, then this is the keyframe we want!
        if(time < (float)animNode->mPositionKeys[i + 1].mTime)
            return i;
    }
    return 0;    //If we don't find anything, just return 0.
}

unsigned int Bone::FindRotation(float time)
{
    for(unsigned int i = 0 ; i < animNode->mNumRotationKeys - 1 ; i++)
    {
        //Same as with the position.
        if(time < (float)animNode->mRotationKeys[i + 1].mTime)
            return i;
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
        aiVector3D assimp_val = animNode->mPositionKeys[0].mValue;
        glm::vec3 val(assimp_val.x,assimp_val.y,assimp_val.z);
        return val;
    }
    std::cout<<"Got here 1"<<std::endl;
    
    
    //The index of our current position, as well as the index that follows.
    //This will allow us to interpolate between the two values.
    unsigned int PositionIndex = FindPosition(time);
    unsigned int NextPositionIndex = (PositionIndex + 1);
    std::cout<<"Got here 2"<<std::endl;
    
    //DeltaTime is the amount of time between the two keyframes.
    float DeltaTime = animNode->mPositionKeys[NextPositionIndex].mTime - animNode->mPositionKeys[PositionIndex].mTime;
    //The factor is simply how much time has passed since the last keyframe,
    //divided by the amount of time between the two keyframes (the DeltaTime)
    //to get the percentage, or how far along between the two keyframes we are.
    float Factor = (time - (float)animNode->mPositionKeys[PositionIndex].mTime) / DeltaTime;
    std::cout<<"Got here 3"<<std::endl;
    
    //The start and end positions (the position values of each of the keyframes)
    const aiVector3D StartPosition = animNode->mPositionKeys[PositionIndex].mValue;
    const aiVector3D EndPosition = animNode->mPositionKeys[NextPositionIndex].mValue;
    std::cout<<"Got here 4"<<std::endl;
    
    //Here we convert them to glm matrices...
    glm::vec3 p1(StartPosition.x,StartPosition.y,StartPosition.z);
    glm::vec3 p2(EndPosition.x,EndPosition.y,EndPosition.z);
    
    std::cout<<"Got here 5"<<std::endl;
    
    
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
        glm::quat val(assimp_val.w,assimp_val.x,assimp_val.y,assimp_val.z);
        return val;
    }
    
    unsigned int RotationIndex = FindRotation(time);
    unsigned int NextRotationIndex = (RotationIndex + 1);
    
    float DeltaTime = animNode->mRotationKeys[NextRotationIndex].mTime - animNode->mRotationKeys[RotationIndex].mTime;
    float Factor = (time - (float)animNode->mRotationKeys[RotationIndex].mTime) / DeltaTime;
    
    const aiQuaternion& StartRotationQ = animNode->mRotationKeys[RotationIndex].mValue;
    const aiQuaternion& EndRotationQ = animNode->mRotationKeys[NextRotationIndex].mValue;
    
    glm::quat r1(StartRotationQ.w,StartRotationQ.x,StartRotationQ.y,StartRotationQ.z);
    glm::quat r2(EndRotationQ.w,EndRotationQ.x,EndRotationQ.y,EndRotationQ.z);
    
    glm::quat val = glm::slerp(r1,r2,Factor);
    return val;
}

//This function will update our node's transformation to match that of
//our keyframes.
void Bone::UpdateKeyframeTransform(float time)
{
    if(animNode == nullptr)
        return;
    
    pos = CalcInterpolatedPosition(time);
    rot = CalcInterpolatedRotation(time);
    scale = glm::vec3(1.0);    //You could add support for scaling, but that's
    //beyond the scope of this tutorial series.
    
    
    glm::mat4 mat;
    mat *= glm::translate(pos);
    mat *= glm::mat4_cast(rot);
    mat *= glm::scale(scale);
    
    node->mTransformation = GLMMat4ToAi(mat);
}
