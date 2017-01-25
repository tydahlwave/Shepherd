#pragma once
#ifndef _SHAPE_H_
#define _SHAPE_H_

#include <string>
#include <vector>
#include <memory>

#include "MatrixStack.h"
#include "ECS2/Bounds.h"

class Program;

class Shape {
public:
	Shape();
	virtual ~Shape();
    
    Bounds *bounds;
    
	void loadMesh(const std::string &meshName);
	void init();
	void resize();
	void draw(Program *prog) const;
	void computeBoundingBox();
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
};

#endif
