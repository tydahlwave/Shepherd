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
// Assimp model loading library
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Mesh.h"
#include "Bounds.h"

class Program;

class Model {
public:
    Model(std::string path);
    
    Bounds bounds;
    
    void draw(Program *shader);//Shader shader);
private:
    std::vector<Mesh> meshes;
    // Stores all the textures loaded so far, optimization to make sure textures aren't loaded more than once.
    std::vector<Texture2D> textures_loaded;
    std::string directory;
    
    void loadModel(std::string path);
    void resize();
    void calculateBounds();
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    std::vector<Texture2D> loadMaterialTextures(aiMaterial* mat, aiTextureType type, std::string typeName);
    GLint TextureFromFile(const char* path, std::string directory);
};

#endif /* Model_h */
