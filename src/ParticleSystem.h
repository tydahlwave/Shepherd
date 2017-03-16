#ifndef ParticleSystem_h
#define ParticleSystem_h

#include "World.h"
//#include "GL\glew.h"
#include "Program.h"
#include "GLSL.h"
#include "Shader.h"
#include "Texture.h"
#include "GLFW\glfw3.h"

#include <algorithm>
#include <random>
#include <glm/gtx/norm.hpp>

struct Particle {
	glm::vec3 position;
	glm::vec3 velocity;
	glm::vec4 color;
	float life;
	float scale;
	float cameraDistance;

	bool operator<(Particle& that) {
		// Sort in reverse order : far particles drawn first
		return this->cameraDistance > that.cameraDistance;
	}

	Particle() : position(0.0f), velocity(0), color(glm::vec4(1.0f, 1.0f, 1.0f, 0.5f)), life(0.0f), scale(1.0f), cameraDistance(0.0f) { }

};

class ParticleSystem {
public:
	ParticleSystem(std::string name, int num, float size, float life, float systemLife, float sp, glm::vec3 pos);
	~ParticleSystem();

	std::string name;
	float scale;
	float systemLife;
	float hasTexture;
	int particlesCount = 0;
	int numParticles;
	GLuint vaoID;
	GLuint billboard_vertex_buffer;
	GLuint particles_position_buffer;
	GLuint particles_color_buffer;
	std::vector<Particle> particles;

	void Init();
	void Update(World &world, float deltaTime, glm::vec3 cameraPos);
	void GenerateParticles(float deltaTime);
	void EmitParticle(int unusedParticle);
	void SortParticles();
	void UpdateVBOs();
	void AddInstancedAttribute();
	void setPositionRange(glm::vec2 xRange, glm::vec2 yRange, glm::vec2 zRange);
	void setVelocityRange(glm::vec2 xRange, glm::vec2 yRange, glm::vec2 zRange);
	int FindUnusedParticle();
	void setPosition(glm::vec3 position);
	void setColor(glm::vec4 color);

private:
	int lastUsedParticle = 0;
	float idealDeltaTime;
	float life;
	float speed;
	glm::vec3 position;
	glm::vec4 color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
	glm::vec2 x_pos_range, x_vel_range;
	glm::vec2 y_pos_range, y_vel_range;
	glm::vec2 z_pos_range, z_vel_range;
	std::vector<float> posBuf;
	std::vector<float> colorBuf;

};

#endif