#include "ParticleSystem.h"
#include "EntityFactory.h"


ParticleSystem::ParticleSystem(std::string name, int num, float scale, float life, float systemLife, float speed, glm::vec3 position)
{	
	this->name = name;
	this->numParticles = num;
	this->scale = scale;
	this->life = life;
	this->systemLife = systemLife;
	this->speed = speed;
	this->position = position;

	Init();
}


ParticleSystem::~ParticleSystem()
{
}

void ParticleSystem::Init() {
	for (int i = 0; i < numParticles; i++) {
		particles.push_back(Particle());
	}

	for (int i = 0; i < numParticles * 3; i++) {
		posBuf.push_back(0);
	}

	for (int i = 0; i < numParticles * 4; i++) {
		colorBuf.push_back(0);
	}
	
	static const GLfloat vertex_buffer_data[] = {
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f,
	};
	glGenBuffers(1, &billboard_vertex_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glBufferData(GL_ARRAY_BUFFER, 12 * sizeof(GLfloat), vertex_buffer_data, GL_STATIC_DRAW);

	glGenBuffers(1, &particles_position_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, numParticles * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &particles_color_buffer);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, numParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
}

void ParticleSystem::Update(World &world, float deltaTime, glm::vec3 cameraPos) {
	if (systemLife == -1.0f || systemLife > 0.0f) {
		if (systemLife > 0.0f) {
			systemLife -= deltaTime;
		}

		GenerateParticles(deltaTime);

		for (int i = 0; i < numParticles; i++) {
			if (particles[i].life > 0.0f) {
				particles[i].life -= deltaTime;

				if (particles[i].life > 0.0f) {
					particles[i].velocity *= 1.01f;
					particles[i].position += particles[i].velocity * deltaTime;
					particles[i].cameraDistance = glm::length2(particles[i].position - cameraPos);
					//particles[i].color.w -= 1.0f / (particles[i].life / deltaTime);
					//if (particles[i].color.w < 0.1f) {
					//	particles[i].color.w = 0.1f;
					//}

					posBuf[3 * i + 0] = particles[i].position.x;
					posBuf[3 * i + 1] = particles[i].position.y;
					posBuf[3 * i + 2] = particles[i].position.z;

					colorBuf[4 * i + 0] = particles[i].color.x;
					colorBuf[4 * i + 1] = particles[i].color.y;
					colorBuf[4 * i + 2] = particles[i].color.z;
					colorBuf[4 * i + 3] = particles[i].color.w;

				}
				else {
					particles[i].cameraDistance = -1.0f;
					EmitParticle(i);
				}
			}
		}

		SortParticles();
		UpdateVBOs();
	}
}

void ParticleSystem::GenerateParticles(float deltaTime) {
	float particlesToCreate = numParticles * deltaTime;
	int count = (int)floor(particlesToCreate);
	if (count == 0) { count = numParticles; }
	for (int i = 0; i < count; ++i) {
		int unusedParticle = FindUnusedParticle();
		EmitParticle(unusedParticle);
	}
}

void ParticleSystem::EmitParticle(int unusedParticle) {
	std::random_device rd;
	std::mt19937 pos(rd());
	std::uniform_real_distribution<float> rand_xPos(x_pos_range.x, x_pos_range.y);
	std::uniform_real_distribution<float> rand_yPos(y_pos_range.x, y_pos_range.y);
	std::uniform_real_distribution<float> rand_zPos(z_pos_range.x, z_pos_range.y);
	glm::vec3 offset = glm::vec3(rand_xPos(pos), rand_yPos(pos), rand_zPos(pos));

	std::mt19937 vel(rd());
	std::uniform_real_distribution<float> rand_xVel(x_vel_range.x, x_vel_range.y);
	std::uniform_real_distribution<float> rand_yVel(y_vel_range.x, y_vel_range.y);
	std::uniform_real_distribution<float> rand_zVel(z_vel_range.x, z_vel_range.y);
	glm::vec3 velocity = glm::vec3(rand_xVel(vel), rand_yVel(vel), rand_zVel(vel));

	velocity = glm::normalize(velocity) * speed;
	GLfloat rColor = 0.5 + ((rand() % 100) / 100.0f);

	std::mt19937 span(rd());
	std::uniform_real_distribution<float> randLife(1.0f, life);

	particles[unusedParticle].position = position + offset;
	particles[unusedParticle].velocity = velocity;
	particles[unusedParticle].life = life;
	particles[unusedParticle].scale = scale;
	particles[unusedParticle].color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
}

void ParticleSystem::SortParticles() {
	//std::sort(&particles[0], &particles[numParticles]);
}

void ParticleSystem::UpdateVBOs() {
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glBufferData(GL_ARRAY_BUFFER, numParticles * 3 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles * 3 * sizeof(GLfloat), &posBuf[0]);

	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glBufferData(GL_ARRAY_BUFFER, numParticles * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW);
	glBufferSubData(GL_ARRAY_BUFFER, 0, numParticles * 4 * sizeof(GLfloat), &colorBuf[0]);
}

void ParticleSystem::AddInstancedAttribute() {
	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, billboard_vertex_buffer);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, particles_position_buffer);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glEnableVertexAttribArray(2);
	glBindBuffer(GL_ARRAY_BUFFER, particles_color_buffer);
	glVertexAttribPointer(2, 4, GL_FLOAT, GL_FALSE, 0, (void*)0);

	glVertexAttribDivisor(0, 0);
	glVertexAttribDivisor(1, 1);
	glVertexAttribDivisor(2, 1);

	//unbind buffers
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
}

void ParticleSystem::setPositionRange(glm::vec2 xRange, glm::vec2 yRange, glm::vec2 zRange) {
	x_pos_range = xRange;
	y_pos_range = yRange;
	z_pos_range = zRange;
}

void ParticleSystem::setVelocityRange(glm::vec2 xRange, glm::vec2 yRange, glm::vec2 zRange) {
	x_vel_range = xRange;
	y_vel_range = yRange;
	z_vel_range = zRange;
}

int ParticleSystem::FindUnusedParticle() {
	for (int i = lastUsedParticle; i < numParticles; ++i) {
		if (particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < lastUsedParticle; ++i) {
		if (particles[i].life <= 0.0f) {
			lastUsedParticle = i;
			return i;
		}
	}

	lastUsedParticle = 0;
	return 0;
}
