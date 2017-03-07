//
//  Renderer.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

// value_ptr for glm
#include <iostream>
#include <glm/gtc/type_ptr.hpp>
#include "GLSL.h"

#include "Renderer.h"
#include "Components/MeshRenderer.h"
#include "Components/Camera.h"
#include "Components/TerrainRenderer.h"
#include "Components/SkyboxRenderer.h"
#include "Components/PathRenderer.h"
#include "Components/Light.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "PhysicsController.h"
#include "Time.h"

Renderer::Renderer() {
	printf("Rendering..\n");
    Initialize();
}

void applyProjectionMatrix(Program *program, Window &window, Camera *camera) {
    MatrixStack stack = MatrixStack();
    float aspectRatio = window.GetWidth()/(float)window.GetHeight();
    stack.perspective(45.0f, aspectRatio, 0.01f, 1000.0f);
    glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(stack.topMatrix()));
}

void applyCameraMatrix(Program *program, Camera *camera, glm::vec3 position) {
    MatrixStack stack = MatrixStack();
    stack.lookAt(position, camera->lookAt, camera->up);
    glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(stack.topMatrix()));
}

void applyTransformMatrix(Program *program, Transform *transform) {
    glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, value_ptr(transform->GetMatrix()));
}

void applyMaterial(Program *program, Material *material) {
    glm::vec4 ambient = material->ambientColor->value;
    glm::vec4 diffuse = material->diffuseColor->value;
    glm::vec4 specular = material->specularColor->value;
    glUniform3f(program->getUniform("matAmbientColor"), ambient.r, ambient.g, ambient.b);
    glUniform3f(program->getUniform("matDiffuseColor"), diffuse.r, diffuse.g, diffuse.b);
    glUniform3f(program->getUniform("matSpecularColor"), specular.r, specular.g, specular.b);
    glUniform1f(program->getUniform("matShine"), material->shine);
}

void Renderer::Initialize() {
    GLSL::checkVersion();
    // Set background color.
    glClearColor(0.0f, 170/255.0f, 1.0f, 1.0f);
    // Enable z-buffer test.
    glEnable(GL_DEPTH_TEST);
}

std::vector<LightStruct> setUpLights(World &world, Path *path) {
    std::vector<LightStruct> lights;
  
    LightStruct spotlight;
    spotlight.position = glm::vec4(-4,50,10,1);
    spotlight.intensities = glm::vec3(2, 2, 2); //strong white light
    spotlight.attenuation = 0.1f;
    spotlight.ambientCoefficient = 0.0f; //no ambient light
    spotlight.coneAngle = 45.0f;
    spotlight.coneDirection = glm::vec3(0,-1,0);
//    lights.push_back(spotlight);
    
    // Spotlight on the player
    //    Light playerLight;
    //    playerLight.position = glm::vec4(world.mainCamera->transform->GetPosition(), 1);
    //    playerLight.intensities = glm::vec3(2,2,2); //strong white light
    //    playerLight.attenuation = 0.1f;
    //    playerLight.ambientCoefficient = 0.0f; //no ambient light
    //    playerLight.coneAngle = 15.0f;
    //    playerLight.coneDirection = glm::vec3(0,-1,0);
    //    lights.push_back(playerLight);
    
    // Add lights at path nodes
    if (path) {
        glm::vec3 nodePos = path->GetNodes()[path->size-1];
        nodePos.y += 20.0f;
//        for (glm::vec3 nodePos : path->GetNodes()) {
            LightStruct nodeLight;
            nodeLight.position = glm::vec4(nodePos, 1);
            nodeLight.intensities = glm::vec3(2, 2, 2); //strong white light
            nodeLight.attenuation = 0.1f;
            nodeLight.ambientCoefficient = 0.0f; //no ambient light
            nodeLight.coneAngle = 15.0f;
            nodeLight.coneDirection = glm::vec3(0,-1,0);
            lights.push_back(nodeLight);
//        }
    }
    
    LightStruct directionalLight;
    directionalLight.position = glm::vec4(1, 0.8, 0.6, 0); //w == 0 indications a directional light
    directionalLight.intensities = glm::vec3(2, 2, 2); //weak yellowish light
    directionalLight.ambientCoefficient = 0.15f;
    lights.push_back(directionalLight);
    
    return lights;
}

bool intersectFrustumAABB(Camera *cam, vec3 min, vec3 max) {
    // Indexed for the 'index trick' later
    vec3 box[] = {min, max};
    
    // We only need to do 6 point-plane tests
    for (int i = 0; i < 6; ++i)
    {
        // This is the current plane
        vec4 plane = cam->planes[i];
        
        // p-vertex selection (with the index trick)
        // According to the plane normal we can know the
        // indices of the positive vertex
        const int px = static_cast<int>(plane.x > 0.0f);
        const int py = static_cast<int>(plane.y > 0.0f);
        const int pz = static_cast<int>(plane.z > 0.0f);
        
        // Dot product
        // project p-vertex on plane normal
        // (How far is p-vertex from the origin)
        const float dp =
        (plane.x*box[px].x) +
        (plane.y*box[py].y) +
        (plane.z*box[pz].z);
        
        // Doesn't intersect if it is behind the plane
        if (dp < -plane.w) { return false; }
    }
    return true;
}

void Renderer::Render(World &world, Window &window) {
    glViewport(0, 0, window.GetWidth(), window.GetHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glEnable(GL_DEPTH_TEST);
    
    Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
    //GLfloat P[16]; //glm::make_mat4(P)
    //glGetFloatv(GL_PROJECTION_MATRIX, P);
    float aspectRatio = window.GetWidth()/(float)window.GetHeight();
    mat4 P = glm::perspective(45.0f, aspectRatio, 0.01f, 1000.0f);
    mat4 V = glm::lookAt(camera->pos, camera->lookAt, camera->up);
    
    camera->ExtractVFPlanes(P, V);
    
    // Find the path from the game objects
    Path *path = nullptr;
    for (GameObject *gameObject : world.GetGameObjects()) {
        PathRenderer *pathRenderer = (PathRenderer*)gameObject->GetComponent("PathRenderer");
        if (pathRenderer && pathRenderer->path) {
            path = pathRenderer->path;
        }
    }
    
    std::vector<LightStruct> lights = setUpLights(world, path);
    for (GameObject *gameObject : world.GetGameObjects()) {
        Light *light = (Light*)gameObject->GetComponent("Light");
        if (light) {
            lights.push_back(light->GetLight());
        }
    }
    
    for (GameObject *gameObject : world.GetGameObjects()) {
		SkyboxRenderer *skyboxRenderer = (SkyboxRenderer*)gameObject->GetComponent("SkyboxRenderer");
		if (skyboxRenderer) {
			auto skybox = skyboxRenderer->skybox;
			auto shader = skyboxRenderer->shader->program;
			auto model = skyboxRenderer->model;

			shader->bind();
			glDepthMask(GL_FALSE);
			glDepthRange(1, 1);
			glDepthFunc(GL_LEQUAL);
			glUniform1i(shader->getUniform("skybox"), 2);
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_CUBE_MAP, skybox->cubeMapTexture);

			Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
			applyProjectionMatrix(shader, window, camera);
			applyCameraMatrix(shader, camera, camera->pos);
			applyTransformMatrix(shader, gameObject->transform);

			model->draw(shader);
		    glDepthMask(GL_TRUE);
			glDepthRange(0, 1);
			glDepthFunc(GL_LESS);
			shader->unbind();
		}
        
        PathRenderer *pathRenderer = (PathRenderer*)gameObject->GetComponent("PathRenderer");
        if (pathRenderer && pathRenderer->path) {
            Path *path = pathRenderer->path;
            for (glm::vec3 nodePos : path->GetNodes()) {
                auto shader = ShaderLibrary::phong->program;
                shader->bind();
                
                applyMaterial(shader, MaterialLibrary::emerald);
                Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
                applyProjectionMatrix(shader, window, camera);
                applyCameraMatrix(shader, camera, camera->pos);//world.mainCamera->transform->GetPosition());
                applyTransformMatrix(shader, gameObject->transform);
                MatrixStack stack = MatrixStack();
                stack.loadIdentity();
                stack.translate(nodePos);
                stack.scale(glm::vec3(1, 1, 1));
                glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, value_ptr(stack.topMatrix()));
                
                // Send lights to GPU
                if (shader->hasUniform("numLights")) glUniform1i(shader->getUniform("numLights"), lights.size());
                for(int i = 0; i < lights.size(); ++i){
                    std::string uniformName = ShaderLibrary::ConstructLightUniformName("position", i);
                    if (shader->hasUniform(uniformName)) glUniform4f(shader->getUniform(uniformName), lights[i].position.x,lights[i].position.y,lights[i].position.z,lights[i].position.w);
                    uniformName = ShaderLibrary::ConstructLightUniformName("intensities", i);
                    if (shader->hasUniform(uniformName)) glUniform3f(shader->getUniform(uniformName), lights[i].intensities.x,lights[i].intensities.y,lights[i].intensities.z);
                    uniformName = ShaderLibrary::ConstructLightUniformName("attenuation", i);
                    if (shader->hasUniform(uniformName)) glUniform1f(shader->getUniform(uniformName), lights[i].attenuation);
                    uniformName = ShaderLibrary::ConstructLightUniformName("ambientCoefficient", i);
                    if (shader->hasUniform(uniformName)) glUniform1f(shader->getUniform(uniformName), lights[i].ambientCoefficient);
                    uniformName = ShaderLibrary::ConstructLightUniformName("coneAngle", i);
                    if (shader->hasUniform(uniformName)) glUniform1f(shader->getUniform(uniformName), lights[i].coneAngle);
                    uniformName = ShaderLibrary::ConstructLightUniformName("coneDirection", i);
                    if (shader->hasUniform(uniformName)) glUniform3f(shader->getUniform(uniformName), lights[i].coneDirection.x,lights[i].coneDirection.y,lights[i].coneDirection.z);
                }
                
                ModelLibrary::sphere->draw(shader);
                shader->unbind();
            }
        }

        MeshRenderer *meshRenderer = (MeshRenderer*)gameObject->GetComponent("MeshRenderer");
        if (meshRenderer && (gameObject->name.compare("HUD") == 0 || gameObject->name.compare("ChargeBar") == 0)) {
            auto shader = meshRenderer->shader->program;
            auto model = meshRenderer->model;
            shader->bind();
            
            if (shader->hasUniform("charge")) {
                float charge = 0;
                if(PhysicsController::charge){
                    charge =(Time::Now() - PhysicsController::LeftClickPressTime) / 10;
                    //charge = (charge > 1.0) ? 1.0 : charge;
                }
                glUniform1f(shader->getUniform("charge"), charge);
                
            }
            
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            applyProjectionMatrix(shader, window, camera);
            applyCameraMatrix(shader, camera, camera->pos);
            applyTransformMatrix(shader, gameObject->transform);
            
            model->draw(shader);
            shader->unbind();
        } else if (meshRenderer && intersectFrustumAABB(camera, gameObject->getBounds().getMin(), gameObject->getBounds().getMax())) {
            auto shader = meshRenderer->shader->program;
            auto model = meshRenderer->model;
            shader->bind();
            
            if (meshRenderer->material) {
                applyMaterial(shader, meshRenderer->material);
            }
            // all of this till next comment will be taken out
//            if (shader->hasUniform("lightPos")) glUniform3f(shader->getUniform("lightPos"), 5, 5, 5);
//            if (shader->hasUniform("lightColor")) glUniform3f(shader->getUniform("lightColor"), 0, 0, 1);
//            if (shader->hasUniform("sunDir")) glUniform3f(shader->getUniform("sunDir"), 0, 1, 0);
//            if (shader->hasUniform("sunColor")) glUniform3f(shader->getUniform("sunColor"), 1, 1, 1);
            
            // all of this till next comment will be taken out
            
            if (shader->hasUniform("numLights")) glUniform1i(shader->getUniform("numLights"), lights.size());
            
            for(int i = 0; i < lights.size(); ++i){
                std::string uniformName = ShaderLibrary::ConstructLightUniformName("position", i);
                if (shader->hasUniform(uniformName)) glUniform4f(shader->getUniform(uniformName), lights[i].position.x,lights[i].position.y,lights[i].position.z,lights[i].position.w);
                uniformName = ShaderLibrary::ConstructLightUniformName("intensities", i);
                if (shader->hasUniform(uniformName)) glUniform3f(shader->getUniform(uniformName), lights[i].intensities.x,lights[i].intensities.y,lights[i].intensities.z);
                uniformName = ShaderLibrary::ConstructLightUniformName("attenuation", i);
                if (shader->hasUniform(uniformName)) glUniform1f(shader->getUniform(uniformName), lights[i].attenuation);
                uniformName = ShaderLibrary::ConstructLightUniformName("ambientCoefficient", i);
                if (shader->hasUniform(uniformName)) glUniform1f(shader->getUniform(uniformName), lights[i].ambientCoefficient);
                uniformName = ShaderLibrary::ConstructLightUniformName("coneAngle", i);
                if (shader->hasUniform(uniformName)) glUniform1f(shader->getUniform(uniformName), lights[i].coneAngle);
                uniformName = ShaderLibrary::ConstructLightUniformName("coneDirection", i);
                if (shader->hasUniform(uniformName)) glUniform3f(shader->getUniform(uniformName), lights[i].coneDirection.x,lights[i].coneDirection.y,lights[i].coneDirection.z);
            }

            
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            applyProjectionMatrix(shader, window, camera);
            if (world.mainCharacter) {
                applyCameraMatrix(shader, camera, camera->pos);
            } else {
                applyCameraMatrix(shader, camera, world.mainCamera->transform->GetPosition());
            }
            applyTransformMatrix(shader, gameObject->transform);
            
            model->draw(shader);
            shader->unbind();
            
            shader->bind();
            // If want to show AABBs
            if (Window::drawAABBs) {
                Bounds bounds = meshRenderer->model->bounds.TransformedBounds(gameObject->transform);
                MatrixStack stack = MatrixStack();
                stack.loadIdentity();
                stack.translate(bounds.center);
                stack.scale(bounds.halfwidths);
                glUniformMatrix4fv(shader->getUniform("M"), 1, GL_FALSE, value_ptr(stack.topMatrix()));
                ModelLibrary::cube->draw(shader);
            }
            shader->unbind();
        }
        
        TerrainRenderer *terrainRenderer = (TerrainRenderer*)gameObject->GetComponent("TerrainRenderer");
        if (terrainRenderer) {
            auto shader = terrainRenderer->shader->program;
            auto terrain = terrainRenderer->terrain;
            shader->bind();
            
            if (terrainRenderer->material) {
                applyMaterial(shader, terrainRenderer->material);
            }
//            if (shader->hasUniform("lightPos")) glUniform3f(shader->getUniform("lightPos"), 5, 5, 5);
//            if (shader->hasUniform("lightColor")) glUniform3f(shader->getUniform("lightColor"), 1, 1, 1);
//            if (shader->hasUniform("sunDir")) glUniform3f(shader->getUniform("sunDir"), 0, 1, 0);
//            if (shader->hasUniform("sunColor")) glUniform3f(shader->getUniform("sunColor"), 1, 1, 1);
            if (shader->hasUniform("terrainMin")) glUniform1i(shader->getUniform("terrainMin"), terrain->min + terrainRenderer->gameObject->transform->GetPosition().y);
            if (shader->hasUniform("terrainMax")) glUniform1i(shader->getUniform("terrainMax"), terrain->max + terrainRenderer->gameObject->transform->GetPosition().y);
            
            if (shader->hasUniform("numLights")) glUniform1i(shader->getUniform("numLights"), lights.size());
            
            // Send terrain colors to GPU
            if (shader->hasUniform("regions")) glUniform1fv(shader->getUniform("regions"), terrainRenderer->regions.size(), terrainRenderer->regions.data());
            if (shader->hasUniform("regionColors")) glUniform3fv(shader->getUniform("regionColors"), terrainRenderer->regionColors.size()*3, &terrainRenderer->regionColors[0][0]);
            
            for(int i = 0; i < lights.size(); ++i){
                std::string uniformName = ShaderLibrary::ConstructLightUniformName("position", i);
                if (shader->hasUniform(uniformName)) glUniform4f(shader->getUniform(uniformName), lights[i].position.x,lights[i].position.y,lights[i].position.z,lights[i].position.w);
                uniformName = ShaderLibrary::ConstructLightUniformName("intensities", i);
                if (shader->hasUniform(uniformName)) glUniform3f(shader->getUniform(uniformName), lights[i].intensities.x,lights[i].intensities.y,lights[i].intensities.z);
                uniformName = ShaderLibrary::ConstructLightUniformName("attenuation", i);
                if (shader->hasUniform(uniformName)) glUniform1f(shader->getUniform(uniformName), lights[i].attenuation);
                uniformName = ShaderLibrary::ConstructLightUniformName("ambientCoefficient", i);
                if (shader->hasUniform(uniformName)) glUniform1f(shader->getUniform(uniformName), lights[i].ambientCoefficient);
                uniformName = ShaderLibrary::ConstructLightUniformName("coneAngle", i);
                if (shader->hasUniform(uniformName)) glUniform1f(shader->getUniform(uniformName), lights[i].coneAngle);
                uniformName = ShaderLibrary::ConstructLightUniformName("coneDirection", i);
                if (shader->hasUniform(uniformName)) glUniform3f(shader->getUniform(uniformName), lights[i].coneDirection.x,lights[i].coneDirection.y,lights[i].coneDirection.z);
            }
            
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            applyProjectionMatrix(shader, window, camera);
            if (world.mainCharacter) {
                applyCameraMatrix(shader, camera, camera->pos);
            } else {
                applyCameraMatrix(shader, camera, world.mainCamera->transform->GetPosition());
            }
            applyTransformMatrix(shader, gameObject->transform);
            
            // Bind all textures for the terrain
            for (int i = 0; i < terrainRenderer->textures.size(); i++) {
                glActiveTexture(GL_TEXTURE0 + i);
                glBindTexture(GL_TEXTURE_2D, terrainRenderer->textures[i]->texID);
                glUniform1i(shader->getUniform(terrainRenderer->textures[i]->name), i);
            }
            terrain->draw(shader);
            
            shader->unbind();
        }
    }
}
