#ifndef HUDRENDERER_H
#define HUDRENDERER_H
#include <iostream>
#include <GLFW/glfw3.h>

#include "Component.h"
#include "../Model.h"
#include "../Material.h"
#include "../Shader.h"
#include "../Texture.h"

class HUDRenderer : public Component {

public:
	HUDRenderer() { name = "HUDRenderer"; };    
	HUDRenderer(Model *t, Shader *s, Material *mat, Texture *tex) : HUDRenderer() { model = t; shader = s; material = mat; texture = tex; }
	virtual ~HUDRenderer() {};

	Model *model = nullptr;
	Shader *shader = nullptr;
	Material *material = nullptr;
	Texture *texture = nullptr;

	bool draw = true;
};
#endif