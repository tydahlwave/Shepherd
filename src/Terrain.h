#pragma once
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <string>
#include <vector>

#include "glm/glm.hpp"

class Program;
class TextureLoader;

class Terrain {
public:
    Terrain();
    virtual ~Terrain();
    
    int size = 256;
    // 0 = Simplex, 1 = Diamond-Square
    int type = 0;
    std::vector<std::vector<float>> heightMap;
    std::vector<float> heightMapFlat;
    
    void Generate();
    void Regenerate();
    void GenerateFromImage(std::string imagePath);
    void draw(Program *prog) const;
    
    TextureLoader *getTexture() { return texture; };
    float getHeight(int row, int col) { return heightMap[row][col]; }
private:
    std::vector<unsigned int> eleBuf;
    std::vector<float> posBuf;
    std::vector<float> norBuf;
    std::vector<float> texBuf;
    unsigned eleBufID;
    unsigned posBufID;
    unsigned norBufID;
    unsigned texBufID;
    unsigned vaoID;
    
    TextureLoader *texture;
    
    void UpdateBuffers();
    void ComputeNormals();
    void init();
    void makeTexture();
};

#endif
