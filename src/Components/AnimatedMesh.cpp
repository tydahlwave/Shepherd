//
//  AnimatedMesh.cpp
//  Shepherd
//
//  Created by Hermes Build Machine on 3/4/17.
//
//

#include "AnimatedMesh.h"
/*
AnimatedMesh::MeshEntry::MeshEntry()
{
    VB = INVALID_OGL_VALUE;
    IB = INVALID_OGL_VALUE;
    NumIndices  = 0;
    MaterialIndex = INVALID_MATERIAL;
};

AnimatedMesh::MeshEntry::~MeshEntry()
{
    if (VB != INVALID_OGL_VALUE)
    {
        glDeleteBuffers(1, &VB);
    }
    
    if (IB != INVALID_OGL_VALUE)
    {
        glDeleteBuffers(1, &IB);
    }
}

void AnimatedMesh::MeshEntry::Init(const std::vector<Vertex>& Vertices,
                           const std::vector<unsigned int>& Indices)
{
    NumIndices = Indices.size();
    
    glGenBuffers(1, &VB);
    glBindBuffer(GL_ARRAY_BUFFER, VB);
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertex) * Vertices.size(), &Vertices[0], GL_STATIC_DRAW);
    
    glGenBuffers(1, &IB);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IB);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(unsigned int) * NumIndices, &Indices[0], GL_STATIC_DRAW);
}

AnimatedMesh::AnimatedMesh()
{
}


AnimatedMesh::~AnimatedMesh()
{
    Clear();
}


void AnimatedMesh::Clear()
{
    for (unsigned int i = 0 ; i < m_Textures.size() ; i++) {
        SAFE_DELETE(m_Textures[i]);
    }
}


bool AnimatedMesh::LoadMesh(const std::string& Filename)
{
    // Release the previously loaded mesh (if it exists)
    Clear();
    
    // Create the VAO
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // Create the buffers for the vertices attributes
    glGenBuffers(1, vbo);

    
    bool Ret = false;
    
    pScene = Importer.ReadFile(Filename.c_str(), aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                   aiProcess_FlipUVs);
    
    if (pScene) {
        globalInverseTransform = pScene->mRootNode->mTransformation;
        globalInverseTransform.Inverse();
        Ret = InitFromScene(pScene, Filename);
    }
    else {
        printf("Error parsing '%s': '%s'\n", Filename.c_str(), Importer.GetErrorString());
    }
    
    // Make sure the VAO is not changed from the outside
    glBindVertexArray(0);
    
    return Ret;
}

bool AnimatedMesh::InitFromScene(const aiScene* pScene, const std::string& Filename)
{
    std::vector<VertexBoneData> Bones;
    
    m_Entries.resize(pScene->mNumMeshes);
    m_Textures.resize(pScene->mNumMaterials);
    
    // Initialize the meshes in the scene one by one
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        const aiMesh* paiMesh = pScene->mMeshes[i];
        InitMesh(i, paiMesh);
        
        //check if mesh has bones
        if(paiMesh->HasBones())
        {
            Bones.resize(paiMesh->mNumVertices);
        }
        else
        {
            return false;
        }
        
        glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
        glBufferData(GL_ARRAY_BUFFER, sizeof(Bones[0]) * Bones.size(), &Bones[0], GL_STATIC_DRAW);
        //bind bones at location 3
        glEnableVertexAttribArray(3);
        glVertexAttribIPointer(3, 4, GL_INT, sizeof(VertexBoneData), (const GLvoid*)0);
        //bind weights at location 4
        glEnableVertexAttribArray(4);
        glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(VertexBoneData), (const GLvoid*)16);
    }
    
    return InitMaterials(pScene, Filename);
}

void AnimatedMesh::InitMesh(unsigned int Index, const aiMesh* paiMesh)
{
    m_Entries[Index].MaterialIndex = paiMesh->mMaterialIndex;
    
    std::vector<Vertex> Vertices;
    std::vector<unsigned int> Indices;
    
    const aiVector3D Zero3D(0.0f, 0.0f, 0.0f);
    
    for (unsigned int i = 0 ; i < paiMesh->mNumVertices ; i++) {
        const aiVector3D* pPos      = &(paiMesh->mVertices[i]);
        const aiVector3D* pNormal   = &(paiMesh->mNormals[i]);
        const aiVector3D* pTexCoord = paiMesh->HasTextureCoords(0) ? &(paiMesh->mTextureCoords[0][i]) : &Zero3D;
        
        Vertex v(vec3(pPos->x, pPos->y, pPos->z),
                 vec3(pNormal->x, pNormal->y, pNormal->z),
                 vec2(pTexCoord->x, pTexCoord->y));
        
        Vertices.push_back(v);
    }
    
    for (unsigned int i = 0 ; i < paiMesh->mNumFaces ; i++) {
        const aiFace& Face = paiMesh->mFaces[i];
        assert(Face.mNumIndices == 3);
        Indices.push_back(Face.mIndices[0]);
        Indices.push_back(Face.mIndices[1]);
        Indices.push_back(Face.mIndices[2]);
    }
    
    m_Entries[Index].Init(Vertices, Indices);
    
    
}


void AnimatedMesh::VertexBoneData::AddBoneData(uint BoneID, float Weight)
{
    for (uint i = 0 ; i < 4 ; i++) {
        if (Weights[i] == 0.0) {
            IDs[i] = BoneID;
            Weights[i] = Weight;
            return;
        }
    }
    
    // should never get here - more bones than we have space for
    assert(0);
}

mat4 AnimatedMesh::BoneTransform(float TimeInSeconds, std::vector<mat4>& Transforms)
{
    mat4 Identity = mat4(1.0);
    
    float TicksPerSecond = pScene->mAnimations[0]->mTicksPerSecond != 0 ?
    pScene->mAnimations[0]->mTicksPerSecond : 25.0f;
    float TimeInTicks = TimeInSeconds * TicksPerSecond;
    float AnimationTime = fmod(TimeInTicks, pScene->mAnimations[0]->mDuration);
    
    ReadNodeHeirarchy(AnimationTime, pScene->mRootNode, Identity);
    
    Transforms.resize(m_NumBones);
    
    for (uint i = 0 ; i < m_NumBones ; i++) {
        Transforms[i] = m_BoneInfo[i].FinalTransformation;
    }
}

void AnimatedMesh::ReadNodeHeirarchy(float AnimationTime, const aiNode* pNode, const mat4& ParentTransform)
{
    std::string NodeName(pNode->mName.data);
    
    const aiAnimation* pAnimation = pScene->mAnimations[0];
    
    aiMatrix4x4 NodeTransformation(pNode->mTransformation);
    
    
    //const aiNodeAnim* pNodeAnim = FindNodeAnim(pAnimation, NodeName);
    const aiNodeAnim* pNodeAnim = *(pAnimation->mChannels);
    
    if (pNodeAnim) {
        // Interpolate scaling and generate scaling transformation matrix
        aiVector3D Scaling;
        CalcInterpolatedScaling(Scaling, AnimationTime, pNodeAnim);
        Matrix4f ScalingM;
        ScalingM.InitScaleTransform(Scaling.x, Scaling.y, Scaling.z);
        
        // Interpolate rotation and generate rotation transformation matrix
        aiQuaternion RotationQ;
        CalcInterpolatedRotation(RotationQ, AnimationTime, pNodeAnim);
        Matrix4f RotationM = Matrix4f(RotationQ.GetMatrix());
        
        // Interpolate translation and generate translation transformation matrix
        aiVector3D Translation;
        CalcInterpolatedPosition(Translation, AnimationTime, pNodeAnim);
        Matrix4f TranslationM;
        TranslationM.InitTranslationTransform(Translation.x, Translation.y, Translation.z);
        
        // Combine the above transformations
        NodeTransformation = TranslationM * RotationM * ScalingM;
    }
    
    Matrix4f GlobalTransformation = ParentTransform * NodeTransformation;
    
    if (m_BoneMapping.find(NodeName) != m_BoneMapping.end()) {
        uint BoneIndex = m_BoneMapping[NodeName];
        m_BoneInfo[BoneIndex].FinalTransformation = m_GlobalInverseTransform * GlobalTransformation *
        m_BoneInfo[BoneIndex].BoneOffset;
    }
    
    for (uint i = 0 ; i < pNode->mNumChildren ; i++) {
        ReadNodeHeirarchy(AnimationTime, pNode->mChildren[i], GlobalTransformation);
    }
}

void AnimatedMesh::LoadBones(uint MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& Bones)
{
    for (uint i = 0 ; i < pMesh->mNumBones ; i++) {
        uint BoneIndex = 0;
        std::string BoneName(pMesh->mBones[i]->mName.data);
        if (boneMap.find(BoneName) == boneMap.end()) {
            BoneIndex = m_NumBones;
            m_NumBones++;
            
            BoneInfo bi;
            m_BoneInfo.push_back(bi);
        }
        else {
            BoneIndex = boneMap[BoneName];
        }
        
        boneMap[BoneName] = BoneIndex;
        boneMap[BoneIndex].BoneOffset = pMesh->mBones[i]->mOffsetMatrix;
        
        for (uint j = 0 ; j < pMesh->mBones[i]->mNumWeights ; j++) {
            uint VertexID = m_Entries[MeshIndex].BaseVertex + pMesh->mBones[i]->mWeights[j].mVertexId;
            float Weight = pMesh->mBones[i]->mWeights[j].mWeight;
            Bones[VertexID].AddBoneData(BoneIndex, Weight);
        }
    } 
}




bool AnimatedMesh::InitMaterials(const aiScene* pScene, const std::string& Filename)
{
    // Extract the directory part from the file name
    /*
    std::string::size_type SlashIndex = Filename.find_last_of("/");
    std::string Dir;
    
    if (SlashIndex == std::string::npos) {
        Dir = ".";
    }
    else if (SlashIndex == 0) {
        Dir = "/";
    }
    else {
        Dir = Filename.substr(0, SlashIndex);
    }
    
    bool Ret = true;
    
    // Initialize the materials
    for (unsigned int i = 0 ; i < pScene->mNumMaterials ; i++) {
        const aiMaterial* pMaterial = pScene->mMaterials[i];
        
        m_Textures[i] = NULL;
        
        if (pMaterial->GetTextureCount(aiTextureType_DIFFUSE) > 0) {
            aiString Path;
            
            if (pMaterial->GetTexture(aiTextureType_DIFFUSE, 0, &Path, NULL, NULL, NULL, NULL, NULL) == AI_SUCCESS) {
                std::string FullPath = Dir + "/" + Path.data;
                m_Textures[i] = new Texture2D(GL_TEXTURE_2D, FullPath.c_str());
                
                if (!m_Textures[i]->Load()) {
                    printf("Error loading texture '%s'\n", FullPath.c_str());
                    delete m_Textures[i];
                    m_Textures[i] = NULL;
                    Ret = false;
                }
                else {
                    printf("Loaded texture '%s'\n", FullPath.c_str());
                }
            }
        }
        
        // Load a white texture in case the model does not include its own texture
        if (!m_Textures[i]) {
            m_Textures[i] = new Texture(GL_TEXTURE_2D, "../Content/white.png");
            
            Ret = m_Textures[i]->Load();
        }
    }
    
    return Ret;
     *//*
    return true;
}

void AnimatedMesh::Render()
{
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    for (unsigned int i = 0 ; i < m_Entries.size() ; i++) {
        glBindBuffer(GL_ARRAY_BUFFER, m_Entries[i].VB);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), 0);
        glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)12);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const GLvoid*)20);
        
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_Entries[i].IB);
        
        const unsigned int MaterialIndex = m_Entries[i].MaterialIndex;
        
        if (MaterialIndex < m_Textures.size() && m_Textures[MaterialIndex]) {
            //m_Textures[MaterialIndex]->Bind(GL_TEXTURE0);
        }
        
        glDrawElements(GL_TRIANGLES, m_Entries[i].NumIndices, GL_UNSIGNED_INT, 0);
    }
    
    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    glDisableVertexAttribArray(2);
}
*/
