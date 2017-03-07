#ifndef CLICKABLE_H
#define CLICKABLE_H
#include <iostream>
#include <GLFW/glfw3.h>

class Shader;
#include "Component.h"
#include "../Shader.h"
#include "../ShaderLibrary.h"

class Clickable : public Component {

public:
	Clickable() { 
		static int nextId = 313;
		name = "Clickable"; 
		id = nextId += 30;
		shader = ShaderLibrary::clickable;
	};
	Shader * shader;
	virtual ~Clickable() {};
	int id;
};

#endif