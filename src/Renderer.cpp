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

Renderer::Renderer() {
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
	glm::vec3 forw = camera->pos + glm::vec3(sin(camera->yaw), 0, cos(camera->yaw));
	forw.y += camera->pitch;
    stack.lookAt(position, forw, camera->up);
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

void Renderer::Render(World &world, Window &window) {
    glViewport(0, 0, window.GetWidth(), window.GetHeight());
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    for (GameObject *gameObject : world.GetGameObjects()) {
        MeshRenderer *meshRenderer = (MeshRenderer*)gameObject->GetComponent("MeshRenderer");
        if (meshRenderer) {
            auto shader = meshRenderer->shader->program;
            auto mesh = meshRenderer->mesh->shape;
            shader->bind();
            
            if (meshRenderer->material) {
                applyMaterial(shader, meshRenderer->material);
            }
            if (shader->hasUniform("lightPos")) glUniform3f(shader->getUniform("lightPos"), 5, 5, 5);
            if (shader->hasUniform("lightColor")) glUniform3f(shader->getUniform("lightColor"), 1, 1, 1);
            if (shader->hasUniform("sunDir")) glUniform3f(shader->getUniform("sunDir"), 0, 1, 0);
            if (shader->hasUniform("sunColor")) glUniform3f(shader->getUniform("sunColor"), 1, 1, 1);
            
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            applyProjectionMatrix(shader, window, camera);
            applyCameraMatrix(shader, camera, camera->pos);
            applyTransformMatrix(shader, gameObject->transform);
            
            mesh->draw(shader);
            
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
            if (shader->hasUniform("lightPos")) glUniform3f(shader->getUniform("lightPos"), 5, 5, 5);
            if (shader->hasUniform("lightColor")) glUniform3f(shader->getUniform("lightColor"), 1, 1, 1);
            if (shader->hasUniform("sunDir")) glUniform3f(shader->getUniform("sunDir"), 0, 1, 0);
            if (shader->hasUniform("sunColor")) glUniform3f(shader->getUniform("sunColor"), 1, 1, 1);
            
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            applyProjectionMatrix(shader, window, camera);
            applyCameraMatrix(shader, camera, camera->pos);
            applyTransformMatrix(shader, gameObject->transform);
            
            terrain->draw(shader);
            
            shader->unbind();
        }
    }
}
