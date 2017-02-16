//
//  Shader.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#include "Shader.h"

Shader *Shader::phong = nullptr;
Shader *Shader::textured = nullptr;
Shader *Shader::hud = nullptr;
Shader *Shader::cell = nullptr;

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
    
    program = new Program();
    program->setVerbose(true);
    program->setShaderNames(resourceDir + "tex_vert.glsl", resourceDir + "tex_frag.glsl");
    program->init();
    program->addUniform("P");
    program->addUniform("M");
    program->addUniform("V");
    program->addUniform("Texture");
    program->addAttribute("vertPos");
    program->addAttribute("vertNor");
    program->addAttribute("vertTex");
    textured = new Shader(program);
    
    program = new Program();
    program->setVerbose(true);
    program->setShaderNames(resourceDir + "hud_vert.glsl", resourceDir + "hud_frag.glsl");
    program->init();
    program->addUniform("P");
    program->addUniform("M");
    program->addUniform("V");
    program->addAttribute("vertPos");
    program->addAttribute("vertNor");
    hud = new Shader(program);
    
    program = new Program();
    program->setVerbose(true);
    program->setShaderNames(resourceDir + "toon_cell_vert.glsl", resourceDir + "toon_cell_frag.glsl");
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
    cell = new Shader(program);
}
