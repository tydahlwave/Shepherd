//
//  ShaderLibrary.h
//  Shepherd
//
//  Created by Tyler Dahl on 2/9/17.
//
//

#ifndef ShaderLibrary_h
#define ShaderLibrary_h

#include <string>

#include "Shader.h"

class ShaderLibrary {
public:
    static void LoadShaders(std::string resourceDir);
    static Shader *phong;
    static Shader *textured;
    static Shader *hud;
    static Shader *cell;
    static Shader *chargeBar;
};

#endif /* ShaderLibrary_h */
