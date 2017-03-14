//
//  Mesher.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 2/9/17.
//
//

#include "Mesh.h"

#include <sstream>
#include <string>

#include "Program.h"

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<GLuint> indices, std::vector<Texture2D> textures) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    
//    this->resize();
//    this->calculateBounds();
//    this->setupMesh();
}

void Mesh::draw(Program *shader) {
    // Bind diffuse and specular textures
//    GLuint diffuseNr = 1;
//    GLuint specularNr = 1;
//    for (GLuint i = 0; i < this->textures.size(); i++) {
//        glActiveTexture(GL_TEXTURE0 + i); // Activate proper texture unit before binding
//        
//        // Retrieve texture number (the N in diffuse_textureN)
//        GLuint number = 0;
//        std::string type = this->textures[i].type;
//        std::string name;
//        
//        if (type == "texture_diffuse")       number = diffuseNr++;
//        else if (type == "texture_specular") number = specularNr++;
//        name = "material." + type + std::to_string(number);
//        
//        glUniform1f(glGetUniformLocation(shader->getPID(), name.c_str()), i);
//        glBindTexture(GL_TEXTURE_2D, this->textures[i].id);
//    }
//    glActiveTexture(GL_TEXTURE0);
    
    // Draw mesh
    glBindVertexArray(this->VAO);
    glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}
void Mesh::drawHud(Program *shader) {
	glBindVertexArray(this->VAO);
	glDrawElements(GL_TRIANGLES, this->indices.size(), GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);
}


void Mesh::resize() {
    glm::vec3 scale, shift;
    glm::vec3 min = glm::vec3(1, 1, 1) * FLT_MAX;
    glm::vec3 max = glm::vec3(1, 1, 1) * FLT_MIN;
//    float epsilon = 0.001f;
    
    // Find min and max extents
    for (int i = 0; i < vertices.size(); i++) {
        Vertex *v = &vertices[i];
        for (int dim = 0; dim < 3; dim++) {
            if (v->pos[dim] < min[dim]) min[dim] = v->pos[dim];
            if (v->pos[dim] > max[dim]) max[dim] = v->pos[dim];
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
    for (int i = 0; i < vertices.size(); i++) {
        Vertex *v = &vertices[i];
        for (int dim = 0; dim < 3; dim++) {
            v->pos[dim] = (v->pos[dim] - shift[dim]) * scale[dim];
//            assert(v->pos[dim] >= -1.0 - epsilon);
//            assert(v->pos[dim] <= 1.0 + epsilon);
        }
    }
}

void Mesh::calculateBounds() {
    glm::vec3 min = glm::vec3(1, 1, 1) * FLT_MAX;
    glm::vec3 max = glm::vec3(1, 1, 1) * FLT_MIN;
    
    // Find min and max extents
    for (Vertex v : vertices) {
        for (int dim = 0; dim < 3; dim++) {
            if (v.pos[dim] < min[dim]) min[dim] = v.pos[dim];
            if (v.pos[dim] > max[dim]) max[dim] = v.pos[dim];
        }
    }
    
    // Set bounds
    this->bounds = Bounds(min, max);
}

void Mesh::setupMesh() {
    // Initialize the vertex array object
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);
    
    // Send the vertex buffer to the GPU
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], GL_STATIC_DRAW);
    
    // Send the element buffer to the GPU
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size()*sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);
    
    // Enable vertex attributes for pos, nor, tex
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    glEnableVertexAttribArray(2);
    
    // Set the vertex attributes for pos, nor, tex
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, nor));
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (GLvoid*)offsetof(Vertex, tex));
    
    // Unbind the vertex array
    glBindVertexArray(0);
}
