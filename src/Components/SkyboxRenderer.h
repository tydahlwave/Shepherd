#ifndef SkyboxRenderer_h
#define SkyboxRenderer_h

#include "Component.h"
#include "../Model.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../Skybox.h"

class SkyboxRenderer : public Component {
public:
    SkyboxRenderer() { name = "SkyboxRenderer"; };
	SkyboxRenderer(Skybox *s, Model *m, Shader *sh, Texture *tex) :SkyboxRenderer() { skybox = s; model = m; shader = sh; texture = tex; }
    virtual ~SkyboxRenderer() {};
    
    Skybox *skybox = nullptr;
	Model *model = nullptr;
	Shader *shader = nullptr;
    Texture *texture = nullptr;
};

#endif