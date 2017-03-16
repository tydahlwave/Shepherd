//
//  Model.h
//  Shepherd
//
//  Created by Tyler Dahl on 2/9/17.
//
//

#ifndef Model_h
#define Model_h

#include <vector>
#include <string>
#include <map>
// Assimp model loading library
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Bounds.h"
#include "Bone.h"

class Program;

class Model {
public:
    Model(std::string path);
    const aiScene* scene;
    Bounds bounds;
    static std::map<std::string, Skeleton> skeletonMap;
    Skeleton skeleton;
    static std::map<std::string, uint> boneMap;
    

    uint numBones = 0;
    
    void draw(Program *shader);//Shader shader);

	void drawHud(Program *shader);//Shader shader);
private:
    //Anim vars
    aiNode* rootNode;
    glm::mat4 globalInverseTransform;
    std::vector<Mesh> meshes;
    std::vector<Bone> bones;
    std::vector<Vertex> vertices;
    
    
    Bone* FindBone(std::string name);
    aiNode* FindAiNode(std::string name);
    aiNodeAnim* FindAiNodeAnim(std::string name);
    AnimationNode* FindNodeAnim(std::string name);
    int FindBoneIDByName(std::string name);
    void recursiveProcess(aiNode* node,const aiScene* scene);
//    void LoadBones(uint MeshIndex, const aiMesh* pMesh, std::vector<VertexBoneData>& boneVData);
//    void ReadNodeHeirarchy(float AnimationTime, aiNode* pNode, const glm::mat4& ParentTransform);
    
    // Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Texture2D> textures_loaded;
    std::string directory;
    std::vector<aiNode*> ai_nodes;
    std::vector<aiNodeAnim*> ai_nodes_anim;
    std::vector<AnimationNode*> animNodes;
    void recursiveNodeProcess(aiNode* node);
    void AnimNodeProcess();
    void loadModel(std::string path);
    void resize();
    void calculateBounds();
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture2D> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    GLint TextureFromFile(const char* path, std::string directory);
};

#endif /* Model_h */
