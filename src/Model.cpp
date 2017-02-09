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
    const aiScene* scene = import.ReadFile(path, aiProcess_Triangulate | aiProcess_GenNormals | aiProcess_FlipUVs);
    
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) {
        std::cout << "ERROR::ASSIMP::" << import.GetErrorString() << std::endl;
        return;
    }
//    this->directory = path.substr(0, path.find_last_of('/')); // Set directory, used in locating textures
    
    // Process meshes
    for (auto i = 0; i < scene->mNumMeshes; i++) {
        aiMesh *mesh = scene->mMeshes[i];
        this->meshes.push_back(this->processMesh(mesh, scene));
    }
    
    // Calculate bounds
    this->calculateBounds();
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
