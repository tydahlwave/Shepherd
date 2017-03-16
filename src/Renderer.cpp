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
#include "Components/Animation.h"
#include "Components/Clickable.h"
#include "Components/HUDRenderer.h"
#include "Components/Button.h"
#include "Components/Light.h"
#include "Components/ParticleRenderer.h"
#include "Components/WaterRenderer.h"
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "PhysicsController.h"
#include "Time.h"
#include "Window.h"

Renderer::Renderer() {
    printf("Rendering..\n");
    Initialize();
}


void applyProjectionMatrix(Program *program, Window &window, Camera *camera) {
    MatrixStack stack = MatrixStack();
    float aspectRatio = window.GetWidth()/(float)window.GetHeight();
    stack.perspective(45.0f, aspectRatio, 0.01f, 5000.0f);
    glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(stack.topMatrix()));
}

void applyOrthographicMatrix(Program *program, Window &window, Camera *camera) {
    MatrixStack stack = MatrixStack();
    stack.ortho2D(0.0, window.GetWidth(), window.GetHeight(), 0.f);
    //stack.ortho2D(0.0, 10.f, 10.f, 0.f);
    glUniformMatrix4fv(program->getUniform("P"), 1, GL_FALSE, value_ptr(stack.topMatrix()));
}


void applyCameraMatrix(Program *program, Camera *camera, glm::vec3 position) {
    MatrixStack stack = MatrixStack();
    stack.lookAt(position, camera->lookAt, camera->up);
	mat4 view = stack.topMatrix();
	if (program->hasUniform("CameraRight_worldspace"))
		glUniform3f(program->getUniform("CameraRight_worldspace"), view[0][0], view[1][0], view[2][0]);
	if (program->hasUniform("CameraUp_worldspace"))
		glUniform3f(program->getUniform("CameraUp_worldspace"), view[0][1], view[1][1], view[2][1]);
    glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(stack.topMatrix()));
}

void applyScreenMatrix(Program *program, Transform *transform, int w, int h) {
    
    MatrixStack stack = MatrixStack();
    stack.loadIdentity();
    glUniformMatrix4fv(program->getUniform("V"), 1, GL_FALSE, value_ptr(stack.topMatrix()));
    glUniformMatrix4fv(program->getUniform("M"), 1, GL_FALSE, value_ptr(transform->GetScreenMatrix(w, h)));
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
    //glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
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
    //    if (path) {
    //        glm::vec3 nodePos = path->GetNodes()[path->size-1];
    //        nodePos.y += 20.0f;
    ////        for (glm::vec3 nodePos : path->GetNodes()) {
    //            LightStruct nodeLight;
    //            nodeLight.position = glm::vec4(nodePos, 1);
    //            nodeLight.intensities = glm::vec3(2, 2, 2); //strong white light
    //            nodeLight.attenuation = 0.1f;
    //            nodeLight.ambientCoefficient = 0.0f; //no ambient light
    //            nodeLight.coneAngle = 15.0f;
    //            nodeLight.coneDirection = glm::vec3(0,-1,0);
    //            lights.push_back(nodeLight);
    ////        }
    //    }
    
    //    LightStruct directionalLight;
    //    directionalLight.position = glm::vec4(-0.6, 0.8, -1, 0); //w == 0 indications a directional light
    //    directionalLight.intensities = glm::vec3(2, 2, 2); //weak yellowish light
    //    directionalLight.ambientCoefficient = 0.15f;
    //    lights.push_back(directionalLight);
    
    return lights;
}

bool Renderer::intersectFrustumAABB(Camera *cam, vec3 min, vec3 max) {
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
    //#define DEBUG
#ifndef DEBUG
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
    
    std::vector<LightStruct> lights;// = setUpLights(world, path);
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
            applyCameraMatrix(shader, camera, (world.mainCharacter) ? camera->pos : world.mainCamera->transform->GetPosition());
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
                
                //ModelLibrary::sphere->draw(shader);
                shader->unbind();
            }
        }
        
        MeshRenderer *meshRenderer = (MeshRenderer*)gameObject->GetComponent("MeshRenderer");
        if (meshRenderer && meshRenderer->draw == false) continue;
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
            if (gameObject->name.compare("HUD") == 0) {
                glUniform1i(shader->getUniform("width"), window.GetWidth());
                glUniform1i(shader->getUniform("height"), window.GetHeight());
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
                uniformName = ShaderLibrary::ConstructLightUniformName("coneDirection", i);
                
            }
            //Pass bones into shader
            if (shader->hasUniform("Bones")){
                Animation* isAnim = (Animation*) gameObject->GetComponent("Animation");
                if(isAnim)
                {
                   // std::cout<<"Mat size for skeleton "<<isAnim->skeleton.boneMats.size()<<std::endl;
                    int i = 0;
//                    for(glm::mat4 m : isAnim->skeleton.boneMats)
//                    {
//                        std::cout<<"BONE #:    "<<i<<std::endl;
//                        printGLMMat4(m);
//                        i++;
//                    }
                    
                    
                    glUniformMatrix4fv(shader->getUniform("Bones"), //We find the location of the gBones uniform.
                                     //If the object is rigged...
                            isAnim->skeleton.boneMats.size(),
                               GL_FALSE,    //We don't need to transpose the matrices.
                               &isAnim->skeleton.boneMats[0][0][0]);
                }
                
            }
            
            
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            applyProjectionMatrix(shader, window, camera);
            if (world.mainCharacter) {
                applyCameraMatrix(shader, camera, camera->pos);
            } else {
                applyCameraMatrix(shader, camera, world.mainCamera->transform->GetPosition());
            }
            
            if(gameObject->name.compare("Camera") == 0 )
            {
                glm::vec3 rot = gameObject->transform->GetRotation();
                Transform t = *new Transform(gameObject->transform->GetPosition(), glm::vec3(rot.x, rot.y - 90, rot.z), gameObject->transform->GetScale());
                applyTransformMatrix(shader, &t);
            }
            else
            {
                applyTransformMatrix(shader, gameObject->transform);
            }
            
            if (meshRenderer->shader == ShaderLibrary::cell || meshRenderer->shader == ShaderLibrary::anim) {
                if (meshRenderer->texture) {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, meshRenderer->texture->texID);
                    glUniform1i(shader->getUniform("myTexture"), 0);
                    glUniform1i(shader->getUniform("useTexture"), 0);
                } else {
                    glActiveTexture(GL_TEXTURE0);
                    glBindTexture(GL_TEXTURE_2D, TextureLibrary::snow->texID);
                    glUniform1i(shader->getUniform("myTexture"), 0);
                    glUniform1i(shader->getUniform("useTexture"), 1);
                }
            }
            
            model->draw(shader);
            shader->unbind();
            
//            if(gameObject->name == "FollowSphere") {
//                GLint viewportArray[4];
//                glGetIntegerv(GL_VIEWPORT, viewportArray);
//                vec4 viewport = vec4(viewportArray[0], viewportArray[1], viewportArray[2], viewportArray[3]);
//                
//                mat4 P = glm::perspective(45.0f, aspectRatio, 0.01f, 1000.0f);
//                mat4 V = glm::lookAt(camera->pos, camera->lookAt, camera->up);
//                
//                vec3 projected = glm::project(gameObject->transform->GetPosition(), V, P, viewport);
//                
//                // now write characters to screen in this projected screen pos
//                
//            }
            
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
            if (shader->hasUniform("terrainScale")) glUniform3f(shader->getUniform("terrainScale"), gameObject->transform->GetScale().x, gameObject->transform->GetScale().y, gameObject->transform->GetScale().z);
            if (shader->hasUniform("useTextureMap")) glUniform1i(shader->getUniform("useTextureMap"), terrain->useTextureMap);
            if (shader->hasUniform("useTextures")) glUniform1i(shader->getUniform("useTextures"), terrain->useTextures);
            
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
                terrainRenderer->textures[i]->bind(i);
                glUniform1i(shader->getUniform(terrainRenderer->textures[i]->name), i);
            }
            terrain->draw();
            
            shader->unbind();
        }

		ParticleRenderer *particleRenderer = (ParticleRenderer*)gameObject->GetComponent("ParticleRenderer");
		if (particleRenderer) {
			auto shader = particleRenderer->shader->program;
			auto ps = particleRenderer->particleSystem;
			auto texture = particleRenderer->texture;

			if (ps->systemLife == -1.0f || ps->systemLife > 0.0f) {
				shader->bind();
				Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
				applyProjectionMatrix(shader, window, camera);
				applyCameraMatrix(shader, camera, camera->pos);
				glUniform1f(shader->getUniform("scale"), ps->scale);
				glUniform1f(shader->getUniform("hasTexture"), ps->hasTexture);

				glUniform1i(shader->getUniform("textureSamp"), 0);
				glActiveTexture(GL_TEXTURE0);
				glBindTexture(GL_TEXTURE_2D, texture->texID);

				glEnable(GL_BLEND);
				glBlendFunc(GL_SRC_ALPHA, GL_ONE);
				ps->AddInstancedAttribute();
				glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, ps->numParticles);
				//glBlendFunc(GL_SRC_ALPHA, GL_BLEND_SRC_ALPHA);
				glDisable(GL_BLEND);
				glDisableVertexAttribArray(0);
				glDisableVertexAttribArray(1);
				glDisableVertexAttribArray(2);

				shader->unbind();
			}
		}

		WaterRenderer *waterRenderer = (WaterRenderer*)gameObject->GetComponent("WaterRenderer");
		if (waterRenderer) {
			auto shader = waterRenderer->shader->program;
			auto waterTile = waterRenderer->waterTile;
			auto model = waterRenderer->model;
			shader->bind();

			//waterTile->moveFactor += waterTile->waveSpeed * 0.016f;
			//fmod(waterTile->moveFactor, 1.0);

			if (waterRenderer->material) {
				applyMaterial(shader, waterRenderer->material);
			}
			if (shader->hasUniform("lightPos")) glUniform3f(shader->getUniform("lightPos"), 5, 5, 5);
			if (shader->hasUniform("lightColor")) glUniform3f(shader->getUniform("lightColor"), 1, 1, 1);
			/*glm::vec4 plane;
			if (waterTile->plane == 0) {
			plane = waterTile->reflectionPlane;
			}
			else {
			plane = waterTile->refractionPlane;
			}
			if (shader->hasUniform("plane")) glUniform4f(shader->getUniform("plane"), plane.x, plane.y, plane.z, plane.w);*/

			glUniform1i(shader->getUniform("reflectionTexture"), 0);
			glUniform1i(shader->getUniform("refractionTexture"), 1);
			glUniform1i(shader->getUniform("dudvMap"), 2);
			glUniform1i(shader->getUniform("normalMap"), 3);
			//glUniform1f(shader->getUniform("moveFactor"), waterTile->moveFactor);
			glm::vec3 pos = world.mainCamera->transform->GetPosition();
			glUniform3f(shader->getUniform("cameraPos"), pos.x, pos.y, pos.z);
			glActiveTexture(GL_TEXTURE0);
			glBindTexture(GL_TEXTURE_2D, waterRenderer->buffers->getReflectionTexture());
			glActiveTexture(GL_TEXTURE1);
			glBindTexture(GL_TEXTURE_2D, waterRenderer->buffers->getRefractionTexture());
			glActiveTexture(GL_TEXTURE2);
			glBindTexture(GL_TEXTURE_2D, waterRenderer->dudv->texID);
			glActiveTexture(GL_TEXTURE3);
			glBindTexture(GL_TEXTURE_2D, waterRenderer->normal->texID);

			Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
			applyProjectionMatrix(shader, window, camera);
			applyCameraMatrix(shader, camera, camera->pos);
			applyTransformMatrix(shader, gameObject->transform);

			model->draw(shader);

			shader->unbind();
		}
    }
    
    glDisable(GL_DEPTH_TEST);
    
    for (GameObject * gameObject : world.GetGameObjects()) {
        HUDRenderer *hr = (HUDRenderer*)gameObject->GetComponent("HudRenderer");
        if (hr && hr->draw) {
            //printf("Renderingo1!!\n");
            auto shader = hr->shader->program;
            auto model = hr->model;
            
            shader->bind();
            
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            //applyProjectionMatrix(shader, window, camera);
            applyOrthographicMatrix(shader, window, camera);
            applyScreenMatrix(shader, gameObject->transform, window.GetWidth(), window.GetHeight());
            //applyTransformMatrix(shader, gameObject->transform);
            
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, hr->texture->texID);
            glUniform1i(shader->getUniform("ButtonTexture"), 0);
            model->draw(shader);
            shader->unbind();
            
        }
    }
    glEnable(GL_DEPTH_TEST);
#else
    glClearColor(0.0f, 0.f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    glViewport(0, 0, window.GetWidth(), window.GetHeight());
    glEnable(GL_DEPTH_TEST);
    
    Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
    //GLfloat P[16]; //glm::make_mat4(P)
    //glGetFloatv(GL_PROJECTION_MATRIX, P);
    float aspectRatio = window.GetWidth() / (float)window.GetHeight();
    mat4 P = glm::perspective(45.0f, aspectRatio, 0.01f, 1000.0f);
    mat4 V = glm::lookAt(camera->pos, camera->lookAt, camera->up);
    
    camera->ExtractVFPlanes(P, V);
    
    
    for (GameObject *gameObject : world.GetGameObjects()) {
        Clickable *cl = (Clickable*)gameObject->GetComponent("Clickable");
        if (!cl)
            continue;
        HUDRenderer *hr = (HUDRenderer*)gameObject->GetComponent("HudRenderer");
        MeshRenderer *mr = (MeshRenderer*)gameObject->GetComponent("MeshRenderer");
        if (hr && hr->draw) {
            if (!hr || hr->draw == false) continue;
            
            
            auto shader = cl->shader->program;
            auto model = hr->model;
            
            
            shader->bind();
            int r = (cl->id & 0x000000FF) >> 0;
            int g = (cl->id & 0x0000FF00) >> 8;
            int b = (cl->id & 0x00FF0000) >> 16;
            glUniform4f(shader->getUniform("PickingColor"), r / 255.f, g / 255.f, b / 255.f, 1.0f);
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            //applyProjectionMatrix(shader, window, camera);
            applyOrthographicMatrix(shader, window, camera);
            applyScreenMatrix(shader, gameObject->transform, window.GetWidth(), window.GetHeight());
            //applyTransformMatrix(shader, gameObject->transform);
            
            model->draw(shader);
            shader->unbind();
        }
        if (mr && mr->draw) {
            auto shader = cl->shader->program;
            auto model = mr->model;
            
            
            shader->bind();
            int r = (cl->id & 0x000000FF) >> 0;
            int g = (cl->id & 0x0000FF00) >> 8;
            int b = (cl->id & 0x00FF0000) >> 16;
            glUniform4f(shader->getUniform("PickingColor"), r / 255.f, g / 255.f, b / 255.f, 1.0f);
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            applyProjectionMatrix(shader, window, camera);
            //applyOrthographicMatrix(shader, window, camera);
            //applyScreenMatrix(shader, gameObject->transform, window.GetWidth(), window.GetHeight());
            if (world.mainCharacter) {
                applyCameraMatrix(shader, camera, camera->pos);
            }
            else {
                applyCameraMatrix(shader, camera, world.mainCamera->transform->GetPosition());
            }
            applyTransformMatrix(shader, gameObject->transform);
            
            model->draw(shader);
            shader->unbind();
        }
    }
#endif
}


int Renderer::checkClickable(World &world, Window &window, int mx, int my) {
    glViewport(0, 0, window.GetWidth(), window.GetHeight());
    glEnable(GL_DEPTH_TEST);
    
    glClearColor(0.0f, 0.f, .0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
    //GLfloat P[16]; //glm::make_mat4(P)
    //glGetFloatv(GL_PROJECTION_MATRIX, P);
    float aspectRatio = window.GetWidth() / (float)window.GetHeight();
    mat4 P = glm::perspective(45.0f, aspectRatio, 0.01f, 1000.0f);
    mat4 V = glm::lookAt(camera->pos, camera->lookAt, camera->up);
    
    camera->ExtractVFPlanes(P, V);
    
    
    for (GameObject *gameObject : world.GetGameObjects()) {
        Clickable *cl = (Clickable*)gameObject->GetComponent("Clickable");
        if (!cl)
            continue;
        HUDRenderer *hr = (HUDRenderer*)gameObject->GetComponent("HudRenderer");
        MeshRenderer *mr = (MeshRenderer*)gameObject->GetComponent("MeshRenderer");
        if (hr && hr->draw) {
            if (!hr || hr->draw == false) continue;
            
            
            auto shader = cl->shader->program;
            auto model = hr->model;
            
            
            shader->bind();
            int r = (cl->id & 0x000000FF) >> 0;
            int g = (cl->id & 0x0000FF00) >> 8;
            int b = (cl->id & 0x00FF0000) >> 16;
            glUniform4f(shader->getUniform("PickingColor"), r / 255.f, g / 255.f, b / 255.f, 1.0f);
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            //applyProjectionMatrix(shader, window, camera);
            applyOrthographicMatrix(shader, window, camera);
            applyScreenMatrix(shader, gameObject->transform, window.GetWidth(), window.GetHeight());
            //applyTransformMatrix(shader, gameObject->transform);
            
            model->draw(shader);
            shader->unbind();
        }
        else if (mr && mr->draw) {
            auto shader = cl->shader->program;
            auto model = mr->model;
            
            
            shader->bind();
            int r = (cl->id & 0x000000FF) >> 0;
            int g = (cl->id & 0x0000FF00) >> 8;
            int b = (cl->id & 0x00FF0000) >> 16;
            glUniform4f(shader->getUniform("PickingColor"), r / 255.f, g / 255.f, b / 255.f, 1.0f);
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            applyProjectionMatrix(shader, window, camera);
            //applyOrthographicMatrix(shader, window, camera);
            //applyScreenMatrix(shader, gameObject->transform, window.GetWidth(), window.GetHeight());
            if (world.mainCharacter) {
                applyCameraMatrix(shader, camera, camera->pos);
            }
            else {
                applyCameraMatrix(shader, camera, world.mainCamera->transform->GetPosition());
            }
            applyTransformMatrix(shader, gameObject->transform);
            
            model->draw(shader);
            shader->unbind();
        }
        
    }
    glFlush();
    glFinish();
    
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    
    unsigned char data[4];
    my = (window.GetHeight() - my);
    glReadPixels(mx, my, 1, 1, GL_RGBA, GL_UNSIGNED_BYTE, data);
    int id = data[0] + data[1] * 256 + data[2] * 256 * 256;
    printf("Clicked! got %d\n", id);
    
    glClearColor(0.0f, 170 / 255.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    this->Render(world, window);
    return id;
}
