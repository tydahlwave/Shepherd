//
//  Shader.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#include "Shader.h"

Shader *Shader::phong = nullptr;

void Shader::LoadShaders(std::string resourceDir) {
    Program *program = new Program();
    program->setVerbose(true);
    program->setShaderNames(resourceDir + "phong_vert.glsl", resourceDir + "phong_frag.glsl");
    program->init();
    program->addUniform("P");
    program->addUniform("M");
    program->addUniform("V");
    program->addAttribute("vertPos");
    program->addAttribute("vertNor");
    program->addUniform("lightPos");
    program->addUniform("lightColor");
    program->addUniform("sunDir");
    program->addUniform("sunColor");
    program->addUniform("matDiffuseColor");
    program->addUniform("matSpecularColor");
    program->addUniform("matAmbientColor");
    program->addUniform("matShine");
    phong = new Shader(program);
}
