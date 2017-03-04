#pragma once
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <string>
#include <vector>

#include "glm/glm.hpp"

#define SIMPLEX_TERRAIN 0
#define DIAMOND_SQUARE_TERRAIN 1

class Program;
class TextureLoader;

class Terrain {
public:
    Terrain();
    virtual ~Terrain();
    
    int size = 256;
    int max = 0;
    int min = 0;
    int type = SIMPLEX_TERRAIN;
    std::vector<std::vector<float>> heightMap;
    
    void Generate();
    void Regenerate();
    void GenerateFromImage(std::string imagePath);
    void UpdateBuffers();
    void init();
    void update();
    void draw(Program *prog) const;
    
    TextureLoader *getTexture() { return texture; };
    float getHeight(int row, int col) { return heightMap[row][col]; }
    std::vector<float> flattenHeightMap() {
        std::vector<float> heightMapFlat;
        for (int i = 0; i < heightMap.size(); i++) {
            for (int j = 0; j < heightMap[i].size(); j++) {
                heightMapFlat.push_back(heightMap[j][i]);
            }
        }
        return heightMapFlat;
    }
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
    
//    void UpdateBuffers();
    void ComputeNormals();
//    void init();
    void makeTexture();
};

#endif
