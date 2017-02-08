#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>

#include "GLSL.h"
#include "MatrixStack.h"
#include "Bounds.h"

class Program;
class TextureLoader;

typedef struct Vertex {
    Vertex(glm::vec3 p, glm::vec3 n, glm::vec2 t) :pos(p), nor(n), tex(t) {};
    glm::vec3 pos;
    glm::vec3 nor;
    glm::vec2 tex;
} Vertex;

typedef struct MeshEntry {
    MeshEntry() {};
    ~MeshEntry() {};
    
    void Init(std::vector<Vertex> &Vertices,
              const std::vector<unsigned int> &Indices);
    
    GLuint VB;
    GLuint IB;
    unsigned int NumIndices;
    unsigned int MaterialIndex;
    Bounds *meshBounds = nullptr;
} MeshEntry;

class Shape {
public:
	Shape();
	virtual ~Shape();
    
    Bounds *bounds;
    
	void loadMesh(const std::string &filePath);
	void init();
	void resize();
	void draw(Program *prog) const;
	void computeBoundingBox();
    void ComputeTex();
private:
	void computeNormals();
    void computeBounds();
	std::vector<unsigned int> eleBuf;
	std::vector<float> posBuf;
	std::vector<float> norBuf;
	std::vector<float> texBuf;
	unsigned eleBufID;
	unsigned posBufID;
	unsigned norBufID;
	unsigned texBufID;
    unsigned vaoID;
    
    std::vector<MeshEntry> m_Entries;
    std::vector<TextureLoader*> m_Textures;
};

#endif
