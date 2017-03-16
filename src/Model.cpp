//
//  Model.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 2/9/17.
//
//

#include "Model.h"

#include <iostream>
// GLSL shader helper
#include "GLSL.h"
// GLM vertex library
#include "glm/glm.hpp"
// Simple OpenGL Image Library
#include "SOIL/Soil.h"

std::map<std::string, Skeleton> Model::skeletonMap;

Model::Model(std::string path) {
    this->loadModel(path);
}

void Model::draw(Program *shader) {
    for(auto i = 0; i < this->meshes.size(); i++) {
        this->meshes[i].draw(shader);
    }
}

void Model::loadModel(std::string path) {
    // Load model using Assimp
    Assimp::Importer import;
    scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);

    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
//    this->directory = path.substr(0, path.find_last_of('/')); // Set directory, used in locating textures
    
    rootNode = scene->mRootNode;
    recursiveNodeProcess(rootNode);
    AnimNodeProcess();
    globalInverseTransform = glm::transpose(AiToGLMMat4(rootNode->mTransformation.Inverse()));
    
    
    // Process meshes
    for (auto i = 0; i < scene->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[i];
        this->meshes.push_back(this->processMesh(mesh, scene));
        bones.clear();
        
        for(int j = 0; j < scene->mMeshes[i]->mNumBones; j++)
        {
            std::string b_name = scene->mMeshes[i]->mBones[j]->mName.data;
            //glm::mat4 b_mat = glm::transpose(AiToGLMMat4(scene->mMeshes[i]->mBones[j]->mOffsetMatrix));
            glm::mat4 b_mat = AiToGLMMat4(scene->mMeshes[i]->mBones[j]->mOffsetMatrix);
            
            //debug
//            std::cout<<"Bone "<<j<<" "<<b_name<<std::endl;
//            std::cout<<"TRANSPOSED OFFSET"<<std::endl;
//            printGLMMat4(b_mat);
//            
//            std::cout<<"Bone "<<j<<" "<<b_name<<std::endl;
//            std::cout<<"TGlobal Inverse OFFSET"<<std::endl;
//            printGLMMat4(globalInverseTransform);
            
            //initial "bone"
            Bone bone(&meshes.at(i),i,b_name,b_mat);
            
            //remaining bone data
            bone.node = new aiNode(*FindAiNode(b_name));
            
            if(bone.node == nullptr)
            {
                std::cout<<"NULL NODE "+b_name<<std::endl;
            }
            else
            {
                std::cout<<"NODE Trans    "+b_name<<std::endl;
                printGLMMat4(AiToGLMMat4((FindAiNode(b_name)->mTransformation)));
            }
            bone.animNode = FindAiNodeAnim(b_name);
            bone.actualAnimNode = FindNodeAnim(b_name);
            
            if(bone.actualAnimNode == nullptr)
                std::cout<<"No Animations were found for "+b_name<<std::endl;
            
            //push the bone into the vector
            bones.push_back(bone);
        }
        
        //collect bone parents
        for(int k = 0; k < bones.size(); k++)
        {
            std::string b_name = bones.at(k).name;
            std::string parent_name = FindAiNode(b_name)->mParent->mName.data;
            
            Bone* p_bone = FindBone(parent_name);
            
            bones.at(k).parent_bone = p_bone;
            
            if(p_bone == nullptr)
                std::cout<<"Parent Bone for "<<" does not exist (is nullptr)"<<std::endl;
            else
                std::cout<<b_name<<" parent is "<<parent_name<<std::endl;
        }
        

        if(meshes.size() > 0)
        {
            std::cout<<"Skeleton set For :"<<scene->mRootNode->mName.data<< "    bone size is :    "<<bones.size()<< "      meshes size is"<< meshes.size() << std::endl;
            skeleton.Init(bones,globalInverseTransform);
        }
            //skeletonMap[path] = Skeleton();
            //skeletonMap[path].Init(bones,globalInverseTransform);

    }

    
    if(!scene->mMeshes[0]->HasBones())
    {
    // Resize entire model
        this->resize();
    }
//    // Resize entire model
//    this->resize();
    
    // Send mesh coords to GPU after resizing entire model
    for (auto i = 0; i < this->meshes.size(); i++) {
        Mesh *mesh = &this->meshes[i];
        mesh->calculateBounds();
        mesh->setupMesh();
    }
    this->calculateBounds();
}


void Model::resize() {
    glm::vec3 scale, shift;
    glm::vec3 min = glm::vec3(1, 1, 1) * FLT_MAX;
    glm::vec3 max = glm::vec3(1, 1, 1) * FLT_MIN;
    float epsilon = 0.001f;
    
    // Find min and max extents
    for (auto i = 0; i < this->meshes.size(); i++) {
        Mesh *mesh = &this->meshes[i];
        for (int i = 0; i < mesh->vertices.size(); i++) {
            Vertex *v = &mesh->vertices[i];
            for (int dim = 0; dim < 3; dim++) {
                if (v->pos[dim] < min[dim]) min[dim] = v->pos[dim];
                if (v->pos[dim] > max[dim]) max[dim] = v->pos[dim];
            }
        }
    }
    
    // Find the longest dimension
    float maxExtent;
    glm::vec3 extent = max - min;
    if (extent[0] >= extent[1] && extent[0] >= extent[2]) maxExtent = extent[0];
    else if (extent[1] >= extent[2]) maxExtent = extent[1];
    else maxExtent = extent[2];
    
    // Calculate the necessary scale and shift for each dimension
    scale = glm::vec3(1, 1, 1) * (2.0f / maxExtent);
    shift = min + (extent / 2.0f);
    
    // Go through all verticies, shift and scale them
    for (auto i = 0; i < this->meshes.size(); i++) {
        Mesh *mesh = &this->meshes[i];
        for (int i = 0; i < mesh->vertices.size(); i++) {
            Vertex *v = &mesh->vertices[i];
            for (int dim = 0; dim < 3; dim++) {
                v->pos[dim] = (v->pos[dim] - shift[dim]) * scale[dim];
                assert(v->pos[dim] >= -1.0 - epsilon);
                assert(v->pos[dim] <= 1.0 + epsilon);
            }
        }
    }
}





void Model::calculateBounds() {
    assert(this->meshes.size() > 0); // Assert model contains at least 1 mesh
    
    // Set model bounds to the first mesh bounds
    bounds = this->meshes[0].bounds;
    
    // Calculate new bounds if model contains multiple meshes
    for (auto i = 1; i < this->meshes.size(); i++) {
        Mesh mesh = this->meshes[i];
        bounds = Bounds::CombinedBounds(bounds, mesh.bounds);
    }
}

Mesh Model::processMesh(aiMesh* mesh, const aiScene* scene) {
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture2D> textures;
    
    // Process vertex positions, normals and texture coordinates
    for (auto i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        vertex.pos = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
        vertex.nor = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
        vertex.tex = mesh->HasTextureCoords(0) ? glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y) : glm::vec2(0, 0);
        vertices.push_back(vertex);
    }
    // Process indices
    for (auto i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];
        for (GLuint j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }
    //populate our bone structures
//    LoadBones(0, mesh, Bones);
    
    
    //Process Bones
    if(mesh->HasBones())
    {
        int WEIGHTS_PER_VERTEX = 4;
        //how many weights allowed per vertex
        
        int boneArraysSize = mesh->mNumVertices*WEIGHTS_PER_VERTEX;
        //4 weights and indices
        std::vector<int> boneIDs;
        boneIDs.resize(boneArraysSize);
        //4 indices per vertex
        std::vector<float> boneWeights;
        boneWeights.resize(boneArraysSize);
        
        
        
        //Cycle through each bone and fill arrays
        for(int i=0;i<mesh->mNumBones;i++)
        {
            //i == current bone
            
            aiBone* aiBone = mesh->mBones[i];
            
            for(int j=0;j<aiBone->mNumWeights;j++)
            {
                aiVertexWeight weight = aiBone->mWeights[j];
                
                //read in the vertex weight
                unsigned int vertexStart = weight.mVertexId * WEIGHTS_PER_VERTEX;
                
                //for each weight per vertex, fill ndx and weight
                for(int k=0;k<WEIGHTS_PER_VERTEX;k++)
                {
                    //empty weight is unfilled
                    if(boneWeights.at(vertexStart+k)==0)
                    {
                        boneWeights.at(vertexStart+k) = weight.mWeight;
                        boneIDs.at(vertexStart+k) = i;
                        //std::cout<<aiBone->mName.data<<" id is "<<i<<"weight is :  "<<weight.mWeight<<std::endl;
                        
                        vertices.at(weight.mVertexId).id[k] = i;
                        //SETTING THE ID
                        //AT k, OF
                        //THE VERTEX AT THIS WEIGHT'S ID,
                        //TO THE CURRENT BONE ID.
                        
                        vertices.at(weight.mVertexId).weight[k] = weight.mWeight;
                        //SETTING THE WEIGHT
                        //AT k, OF
                        //THE VERTEX AT THIS WEIGHT'S ID,
                        //TO THIS WEIGHT'S WEIGHT.
                        break;
                    }
                }
                
            }
        }
    }
    
    // Process material (diffuse and specular textures)
//    if (scene->HasMaterials()) {
//        aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
//        std::vector<Texture> diffuseMaps = this->loadMaterialTextures(material, aiTextureType_DIFFUSE, "texture_diffuse");
//        textures.insert(textures.end(), diffuseMaps.begin(), diffuseMaps.end());
//        std::vector<Texture> specularMaps = this->loadMaterialTextures(material, aiTextureType_SPECULAR, "texture_specular");
//        textures.insert(textures.end(), specularMaps.begin(), specularMaps.end());
//    }
    
    return Mesh(vertices, indices, textures);
}

// Load diffuse and specular textures
//std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName) {
//    std::vector<Texture> textures;
//    for(GLuint i = 0; i < mat->GetTextureCount(type); i++)
//    {
//        aiString str;
//        mat->GetTexture(type, i, &str);
//        GLboolean skip = false;
//        for(GLuint j = 0; j < textures_loaded.size(); j++)
//        {
//            if(textures_loaded[j].path == str)
//            {
//                textures.push_back(textures_loaded[j]);
//                skip = true;
//                break;
//            }
//        }
//        // If texture hasn't been loaded already, load it
//        if (!skip) {
//            Texture texture;
//            texture.id = TextureFromFile(str.C_Str(), this->directory);
//            texture.type = typeName;
//            texture.path = str;
//            textures.push_back(texture);
//            this->textures_loaded.push_back(texture);  // Add to loaded textures
//        }
//    }
//    return textures;
//}
//
//GLint Model::TextureFromFile(const char* path, std::string directory) {
//    //Generate texture ID and load texture data
//    std::string filename = std::string(path);
//    filename = directory + '/' + filename;
//    GLuint textureID;
//    glGenTextures(1, &textureID);
//    int width,height;
//    unsigned char* image = SOIL_load_image(filename.c_str(), &width, &height, 0, SOIL_LOAD_RGB);
//    // Assign texture to ID
//    glBindTexture(GL_TEXTURE_2D, textureID);
//    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, image);
//    glGenerateMipmap(GL_TEXTURE_2D);
//    
//    // Parameters
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR );
//    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//    glBindTexture(GL_TEXTURE_2D, 0);
//    SOIL_free_image_data(image);
//    return textureID;
//}


//NEW ANIM STUFF

//recursively goes through each aiNode in the aiScene, and fills the ai_nodes vector with this aiNode.
void Model::recursiveNodeProcess(aiNode* node)
{
    std::cout<<" NODE name "<<node->mName.C_Str()<<std::endl;
    printGLMMat4(AiToGLMMat4(node->mTransformation)  );
    
    aiNode* placeNode = new aiNode(*node);
    ai_nodes.push_back(placeNode);
    
    for(int i = 0; i < node->mNumChildren; i++)
        recursiveNodeProcess(node->mChildren[i]);
}

//loads the entire scene’s animation data into the ai_nodes_anim vector.
//Each mChannels in the for loop is simply an aiNodeAnim object,
//with the keyframes of its animation, and the name of the aiNode it corresponds to.
void Model::AnimNodeProcess()
{
    if(scene->mNumAnimations == 0)
        return;
    //collect the animations
    aiNodeAnim* temp;
    AnimationNode* tempA;
    std::vector<PositionKey> positionTemp;
    std::vector<RotationKey> rotationTemp;
    //collect the animation frames
    for(int i = 0; i < scene->mAnimations[0]->mNumChannels; i++)
    {
        temp = new aiNodeAnim(*scene->mAnimations[0]->mChannels[i]);
        
        positionTemp.resize(temp->mNumPositionKeys);
        
        for(unsigned int j = 0 ; j < temp->mNumPositionKeys - 1 ; j++)
        {
            //temp->mPositionKeys[j] = *new aiVectorKey(scene->mAnimations[0]->mChannels[i]->mPositionKeys[j]);
            
            aiVectorKey tKey = temp->mPositionKeys[j];
            glm::vec3 tempV = glm::vec3(tKey.mValue.x, tKey.mValue.y, tKey.mValue.z);
            
            positionTemp[j].position = tempV;
            positionTemp[j].startTime = tKey.mTime;
        }
        
        rotationTemp.resize(temp->mNumRotationKeys);
                                   
        for(unsigned int k = 0 ; k < temp->mNumRotationKeys - 1 ; k++)
        {
            //temp->mRotationKeys[k] = *new aiQuatKey(scene->mAnimations[0]->mChannels[i]->mRotationKeys[k]);
            
            aiQuatKey tKey = temp->mRotationKeys[k];
            glm::quat tempQ = glm::quat(tKey.mValue.w, tKey.mValue.x, tKey.mValue.y, tKey.mValue.z);
            
            
            rotationTemp[k].rotation = tempQ;
            rotationTemp[k].startTime = tKey.mTime;
        }
        tempA = new AnimationNode(temp->mNodeName.data, positionTemp, rotationTemp);
        animNodes.push_back(tempA);
        ai_nodes_anim.push_back(temp);

    }
    //ai_nodes_anim.push_back(scene->mAnimations[0]->mChannels[i]);
}

//scan bone vector for name
Bone* Model::FindBone(std::string name)
{
    for(int i = 0; i < bones.size(); i++)
    {
        if(bones.at(i).name == name)
            return &bones.at(i);
    }
    return nullptr;
}

//scande node vector by name
aiNode* Model::FindAiNode(std::string name)
{
    for(int i = 0; i < ai_nodes.size(); i++)
    {
        if(ai_nodes.at(i)->mName.data == name)
            return ai_nodes.at(i);
    }
    
    return nullptr;
}

//find animation node
aiNodeAnim* Model::FindAiNodeAnim(std::string name)
{
    for(int i = 0; i < ai_nodes_anim.size(); i++)
    {
        if(ai_nodes_anim.at(i)->mNodeName.data == name)
        {   //std::cout<<"Found anim node for/:   "<< ai_nodes_anim.at(i)->mNodeName.data << std::endl;
            return ai_nodes_anim.at(i);
        }
    }

    return nullptr;
}

//find animation node
AnimationNode* Model::FindNodeAnim(std::string name)
{
    for(int i = 0; i < animNodes.size(); i++)
    {
        if(animNodes.at(i)->nodeName == name)
        {   std::cout<<"Found anim node for/:   "<< ai_nodes_anim.at(i)->mNodeName.data << std::endl;
            return animNodes.at(i);
        }
    }
    
    return nullptr;
}

//find bone id by name
int Model::FindBoneIDByName(std::string name)
{
    for(int i = 0; i < bones.size(); i++)
    {
        if(bones.at(i).name == name)
            return i;
    }
 
    return -1;
}



//unneeded yet
void Model::recursiveProcess(aiNode* node,const aiScene* scene)
{
    //process
    for(int i=0;i<node->mNumMeshes;i++)
    {
        aiMesh* mesh=scene->mMeshes[node->mMeshes[i]];
        processMesh(mesh,scene);
    }
    
    
    
    //recursion
    for(int i=0;i<node->mNumChildren;i++)
    {
        recursiveProcess(node->mChildren[i],scene);
    }
}
