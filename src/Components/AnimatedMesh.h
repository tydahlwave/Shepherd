//
//  AnimatedMesh.hpp
//  Shepherd
//
//  Created by Hermes Build Machine on 3/4/17.
//
//

#ifndef AnimatedMesh_h
#define AnimatedMesh_h
/*
#include <stdio.h>
#include <map>
#include "../Mesh.h"

class AnimatedMesh
{
public:
    AnimatedMesh();
    
    ~AnimatedMesh();
    
    bool LoadMesh(const std::string& Filename);
    
    void Render();
    
    Assimp::Importer Importer;
    
    const aiScene* pScene;
    
    aiMatrix4x4 globalInverseTransform;
    
    int m_NumBones = 0;
    
    
    GLuint vao, vbo[1];
private:
    #define MAX_BONES_PER_VER 4
    struct VertexBoneData
    {
        uint IDs[MAX_BONES_PER_VER];
        float Weights[MAX_BONES_PER_VER];
        void AddBoneData(uint BoneID, float Weight);
    };

    
    
    std::map<std::string, uint> boneMap;
    mat4 BoneTransform(float TimeInSeconds, std::vector<mat4>& Transforms);
    bool InitFromScene(const aiScene* pScene, const std::string& Filename);
    void InitMesh(unsigned int Index, const aiMesh* paiMesh);
    bool InitMaterials(const aiScene* pScene, const std::string& Filename);
    void LoadBones(uint MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones);
    void ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const mat4& ParentTransform);
    void Clear();
    
#define INVALID_MATERIAL 0xFFFFFFFF

    
    
    struct MeshEntry {
        MeshEntry();
        
        ~MeshEntry();
        
        void Init(const std::vector<Vertex>& Vertices,
                  const std::vector<unsigned int>& Indices);
        
        GLuint VB;
        GLuint IB;
        unsigned int NumIndices;
        unsigned int MaterialIndex;
    };
    
    std::vector<MeshEntry> m_Entries;
    std::vector<Texture2D*> m_Textures;
};


*/
#endif /* AnimatedMesh_h */
