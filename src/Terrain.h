#pragma once
#ifndef _TERRAIN_H_
#define _TERRAIN_H_

#include <string>
#include <vector>

#include "glm/glm.hpp"

class Program;

class Terrain {
public:
    Terrain();
    virtual ~Terrain();
    
    void Generate(int size, int type);
    void init();
    void draw(Program *prog) const;
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
    
    std::vector<std::vector<float>> heightMap;
    
    void ComputeNormals();
};

#endif
