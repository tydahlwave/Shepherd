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
Shader *ShaderLibrary::hud2 = nullptr;
Shader *ShaderLibrary::cell = nullptr;
Shader *ShaderLibrary::chargeBar = nullptr;
Shader *ShaderLibrary::clickable = nullptr;
Shader *ShaderLibrary::ground = nullptr;
Shader *ShaderLibrary::menu = nullptr;
Shader *ShaderLibrary::skybox = nullptr;
Shader *ShaderLibrary::particle = nullptr;
Shader *ShaderLibrary::water = nullptr;
Shader *ShaderLibrary::anim = nullptr;
Shader *ShaderLibrary::inFrontOfCamera = nullptr;

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
	program->addUniform("width");
	program->addUniform("height");
    program->addAttribute("vertPos");
    program->addAttribute("vertNor");
    hud = new Shader(program);

	program = new Program();
	program->setVerbose(true);
	program->setShaderNames(resourceDir + "hud2_vert.glsl", resourceDir + "hud2_frag.glsl");
	program->init();
	program->addUniform("P");
	program->addUniform("M");
	program->addUniform("V");
	program->addUniform("ButtonTexture");
	program->addAttribute("vertPos");
	program->addAttribute("vertNor");
	hud2 = new Shader(program);

	program = new Program();
	program->setVerbose(true);
	program->setShaderNames(resourceDir + "click_vert.glsl", resourceDir + "click_frag.glsl");
	program->init();
	program->addUniform("P");
	program->addUniform("M");
	program->addUniform("V");
	program->addUniform("PickingColor");
	program->addAttribute("vertPos");
	program->addAttribute("vertNor");
	clickable = new Shader(program);

    program = new Program();
    program->setVerbose(true);
    program->setShaderNames(resourceDir + "toon_cell_vert.glsl", resourceDir + "toon_cell_frag.glsl");
    program->init();
    program->addUniform("P");
    program->addUniform("M");
    program->addUniform("V");
    program->addAttribute("vertPos");
    program->addAttribute("vertNor");
    program->addAttribute("vertTex");
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
    program->addUniform("myTexture");
    program->addUniform("useTexture");
    cell = new Shader(program);
    
    ///anim testing
    program = new Program();
    program->setVerbose(true);
    program->setShaderNames(resourceDir + "rigged_toon_vert.glsl", resourceDir + "rigged_toon_frag.glsl");
    program->init();
    program->addUniform("P");
    program->addUniform("M");
    program->addUniform("V");
    
    program->addAttribute("vertPos");
    program->addAttribute("vertNor");
    program->addAttribute("vertID");
    program->addAttribute("vertWeight");
    program->addAttribute("vertTex");
    
    program->addUniform("numLights");
    program->addUniform("Bones");
    
    
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
    program->addUniform("myTexture");
    program->addUniform("useTexture");
    anim = new Shader(program);
    
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
    program->addUniform("terrainScale");
    program->addUniform("regions");
    program->addUniform("regionColors");
    program->addUniform("Grass");
    program->addUniform("Mountain");
    program->addUniform("Snow");
    program->addUniform("useTextureMap");
    program->addUniform("useTextures");
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

	program = new Program();
	program->setVerbose(true);
	program->setShaderNames(resourceDir + "particle_vert.glsl", resourceDir + "particle_frag.glsl");
	program->init();
	program->addUniform("P");
	program->addUniform("V");
	program->addUniform("textureSamp");
	program->addUniform("scale");
	program->addUniform("hasTexture");
	program->addUniform("CameraRight_worldspace");
	program->addUniform("CameraUp_worldspace");
	particle = new Shader(program);

	program = new Program();
	program->setVerbose(true);
	program->setShaderNames(resourceDir + "water_vert.glsl", resourceDir + "water_frag.glsl");
	program->init();
	program->addUniform("P");
	program->addUniform("M");
	program->addUniform("V");
	program->addUniform("lightPos");
	program->addUniform("lightColor");
	//program->addUniform("plane");
	program->addUniform("reflectionTexture");
	program->addUniform("refractionTexture");
	program->addUniform("dudvMap");
	program->addUniform("normalMap");
	program->addUniform("cameraPos");
	//program->addUniform("moveFactor");
	program->addAttribute("vertPos");
	program->addAttribute("vertNor");
	water = new Shader(program);
    
    program = new Program();
    program->setVerbose(true);
    program->setShaderNames(resourceDir + "in_front_of_cam_vert.glsl", resourceDir + "in_front_of_cam_frag.glsl");
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
    inFrontOfCamera = new Shader(program);
}
