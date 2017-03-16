#ifndef WaterTile_h
#define WaterTile_h

#include "Texture.h"
#include "TextureLoader.h"
#include <glm/glm.hpp>

class WaterTile {
public:
	WaterTile();
	WaterTile(float x, float z, float height) : WaterTile() { x = x; z = z; height = height; }
	~WaterTile();

	void InitTexture();

	int plane;
	float tileSize;
	float height;
	glm::vec4 reflectionPlane;
	glm::vec4 refractionPlane;
	float moveFactor = 0;
	float waveSpeed = 0.2;

private:
	float x;
	float z;
};

#endif