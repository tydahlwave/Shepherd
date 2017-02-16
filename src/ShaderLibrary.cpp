//
//  ShaderLibrary.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 2/9/17.
//
//

#include "ShaderLibrary.h"

Shader *ShaderLibrary::phong = nullptr;
Shader *ShaderLibrary::textured = nullptr;
Shader *ShaderLibrary::hud = nullptr;
Shader *ShaderLibrary::cell = nullptr;
Shader *ShaderLibrary::chargeBar = nullptr;
float *ShaderLibrary::charge = 0;

void ShaderLibrary::LoadShaders(std::string resourceDir) {
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
    program->addUniform("numLights");
    program->addUniform("position");
    program->addUniform("intensities");
    program->addUniform("attenuation");
    program->addUniform("ambientCoefficient");
    program->addUniform("coneAngle");
    program->addUniform("coneDirection");
    program->addUniform("matDiffuseColor");
    program->addUniform("matSpecularColor");
    program->addUniform("matAmbientColor");
    program->addUniform("matShine");
    program->addUniform("cameraPos");
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
    
    program = new Program();
    program->setVerbose(true);
    program->setShaderNames(resourceDir + "charge_vert.glsl", resourceDir + "charge_frag.glsl");
    program->init();
    program->addUniform("P");
    program->addUniform("M");
    program->addUniform("V");
    program->addUniform("charge");
    program->addAttribute("vertPos");
    program->addAttribute("vertNor");
    chargeBar = new Shader(program);
}
