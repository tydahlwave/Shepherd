//
//  Shader.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Shader_h
#define Shader_h

#include <string>

#include "Program.h"

class Shader {
public:
    Shader() {};
    Shader(Program *p) { program = p; }
    virtual ~Shader() {};
    
    Program *program = nullptr;
};

#endif /* Shader_h */
