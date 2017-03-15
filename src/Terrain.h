#pragma once
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <string>
#include <vector>

#include <GL/glew.h>
#include "glm/glm.hpp"

#include "Noise/NoiseProperties.h"
#include "Texture.h"

#define SIMPLEX_TERRAIN 0
#define DIAMOND_SQUARE_TERRAIN 1

class Program;
class TextureLoader;

struct TerrainVertex {
    TerrainVertex() {}
    TerrainVertex(glm::vec3 p, glm::vec3 n) :pos(p), nor(n) {}
    glm::vec3 pos = glm::vec3(0, 0, 0);
    glm::vec3 nor = glm::vec3(0, 0, 0);
};

class Terrain {
public:
    Terrain(int size = 256, NoiseProperties props = {});
    Terrain(std::string filePath);
    virtual ~Terrain();
    
    int size = 256;
    float maxHeight = 128;
    time_t seed = 0;
    int width = 0, height = 0;
    float min = 0, max = 0;
    int type = SIMPLEX_TERRAIN;
    Texture heightmapTex;
    
    std::vector<TerrainVertex> vertices;
    std::vector<GLuint> indices;
    std::vector<Texture *> textures;
    
    void generate(int size = 256, NoiseProperties props = {});
    void smooth(int iterations, int kernelSize);
    
    float getHeight(int x, int y);
    void setHeight(int x, int y, float height);
    void *getHeightmap() { return data; }
    
    void init();
    void update();
    void draw();
    
    void createMesh();
private:
    unsigned short *data = nullptr;
    GLuint VAO, VBO, EBO;
    
    void ComputeNormals();
};

#endif
