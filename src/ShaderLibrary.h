//
//  ShaderLibrary.h
//  Shepherd
//
//  Created by Tyler Dahl on 2/9/17.
//
//

#ifndef ShaderLibrary_h
#define ShaderLibrary_h

#include <sstream>
#include <string>

#include "Shader.h"

class ShaderLibrary {
public:
    static void LoadShaders(std::string resourceDir);
    static Shader *phong;
    static Shader *textured;
    static Shader *hud;
	static Shader *hud2;
    static Shader *cell;
	static Shader *clickable;
    static Shader *chargeBar;
    static Shader *ground;
    static Shader *menu;
	static Shader *skybox;
	static Shader *particle;
	static Shader *water;
    static Shader *anim;
    static Shader *inFrontOfCamera;

    static std::string ConstructLightUniformName(const char* propertyName, size_t lightIndex) {
        std::ostringstream ss;
        ss << "allLights[" << lightIndex << "]." << propertyName;
        std::string uniformName = ss.str();
        
        return uniformName;
    }

};

#endif /* ShaderLibrary_h */
