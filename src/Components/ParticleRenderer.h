#ifndef ParticleRenderer_h
#define ParticleRenderer_h

#include "Component.h"
#include "../ParticleSystem.h"
#include "../Shader.h"
#include "../Texture.h"
//#include <math.h>
//#include <vector>

class ParticleRenderer : public Component {
public:
    ParticleRenderer() { name = "ParticleRenderer"; };
	ParticleRenderer(ParticleSystem *ps, Shader *s, Texture *t) : ParticleRenderer() { particleSystem = ps; shader = s; texture = t; }
    virtual ~ParticleRenderer() {};
    
    ParticleSystem *particleSystem = nullptr;
	Shader *shader = nullptr;
	Texture *texture = nullptr;
};

#endif