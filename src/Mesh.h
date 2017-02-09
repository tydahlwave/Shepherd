//
//  Mesh.h
//  Shepherd
//
//  Created by Tyler Dahl on 2/9/17.
//
//

#ifndef Mesh_h
#define Mesh_h

#include <string>
#include <vector>
// GLSL shader helper
#include "GLSL.h"
// GLM vertex library
#include "glm/glm.hpp"
// Assimp model loading library
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include "Bounds.h"

class Program;

struct Vertex {
    Vertex() { pos = nor = glm::vec3(0, 0, 0); tex = glm::vec2(0, 0); }
    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) :pos(p), nor(n), tex(t) {}
    glm::vec3 pos;
    glm::vec3 nor;
    glm::vec2 tex;
};

struct Texture2D {
    GLuint id;
    std::string type;
    aiString path;  // We store the path of the texture to compare with other textures
};

class Mesh {
public:
    Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture2D> textures);

    Bounds bounds;
    
    std::vector<Vertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture2D> textures;
    
    void draw(Program *shader);
private:
    // Render data
    GLuint VAO, VBO, EBO;

    void resize();
    void calculateBounds();
    void setupMesh();
};

#endif /* Mesh_h */
