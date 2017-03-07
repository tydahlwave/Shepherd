//
//  ShaderLibrary.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 2/9/17.
//
//

#include "ShaderLibrary.h"
#define MAX_NUM_LIGHTS 10

Shader *ShaderLibrary::phong = nullptr;
Shader *ShaderLibrary::textured = nullptr;
Shader *ShaderLibrary::hud = nullptr;
Shader *ShaderLibrary::cell = nullptr;
Shader *ShaderLibrary::chargeBar = nullptr;
Shader *ShaderLibrary::ground = nullptr;
Shader *ShaderLibrary::menu = nullptr;
Shader *ShaderLibrary::skybox = nullptr;

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
    program->addUniform("numLights");
    for(int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::string uniformName = ConstructLightUniformName("position", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("intensities", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("attenuation", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("ambientCoefficient", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("coneAngle", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("coneDirection", i);
        program->addUniform(uniformName);
    }
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
    program->addUniform("numLights");
    for(int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::string uniformName = ConstructLightUniformName("position", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("intensities", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("attenuation", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("ambientCoefficient", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("coneAngle", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("coneDirection", i);
        program->addUniform(uniformName);
    }
    program->addUniform("matDiffuseColor");
    program->addUniform("matSpecularColor");
    program->addUniform("matAmbientColor");
    program->addUniform("matShine");
    cell = new Shader(program);
    
    program = new Program();
    program->setVerbose(true);
    program->setShaderNames(resourceDir + "ground_vert.glsl", resourceDir + "terrainGeometryFlat.vs", resourceDir + "ground_frag.glsl");
    program->init();
    program->addUniform("P");
    program->addUniform("M");
    program->addUniform("V");
    program->addAttribute("vertPos");
    program->addAttribute("vertNor");
    program->addUniform("numLights");
    for(int i = 0; i < MAX_NUM_LIGHTS; i++) {
        std::string uniformName = ConstructLightUniformName("position", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("intensities", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("attenuation", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("ambientCoefficient", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("coneAngle", i);
        program->addUniform(uniformName);
        uniformName = ConstructLightUniformName("coneDirection", i);
        program->addUniform(uniformName);
    }
    program->addUniform("terrainMin");
    program->addUniform("terrainMax");
    program->addUniform("regions");
    program->addUniform("regionColors");
    program->addUniform("Grass");
    program->addUniform("Mountain");
    program->addUniform("Snow");
    program->addUniform("matDiffuseColor");
    program->addUniform("matSpecularColor");
    program->addUniform("matAmbientColor");
    program->addUniform("matShine");
    ground = new Shader(program);
    
    program = new Program();
    program->setVerbose(true);
    program->setShaderNames(resourceDir + "menu_vert.glsl", resourceDir + "menu_frag.glsl");
    program->init();
    program->addUniform("P");
    program->addUniform("M");
    program->addUniform("V");
    program->addAttribute("vertPos");
    program->addAttribute("vertNor");
    menu = new Shader(program);
    
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

	program = new Program();
	program->setVerbose(true);
	program->setShaderNames(resourceDir + "skybox_vert.glsl", resourceDir + "skybox_frag.glsl");
	program->init();
	program->addUniform("P");
	program->addUniform("M");
	program->addUniform("V");
	program->addUniform("skybox");
	program->addAttribute("vertPos");
	program->addAttribute("vertNor");
	skybox = new Shader(program);
}
