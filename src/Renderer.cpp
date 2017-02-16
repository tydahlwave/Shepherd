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
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "PhysicsController.h"
#include "Time.h"

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

std::vector<Light> setUpLights() {
    std::vector<Light> lights;

    Light spotlight;
    spotlight.position = glm::vec4(-4,0,10,1);
    spotlight.intensities = glm::vec3(2,2,2); //strong white light
    spotlight.attenuation = 0.1f;
    spotlight.ambientCoefficient = 0.0f; //no ambient light
    spotlight.coneAngle = 15.0f;
    spotlight.coneDirection = glm::vec3(0,0,-1);
    
    Light directionalLight;
    directionalLight.position = glm::vec4(1, 0.8, 0.6, 0); //w == 0 indications a directional light
    directionalLight.intensities = glm::vec3(0.4,0.3,0.1); //weak yellowish light
    directionalLight.ambientCoefficient = 0.06f;
    
    lights.push_back(spotlight);
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
    
    Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
    //GLfloat P[16]; //glm::make_mat4(P)
    //glGetFloatv(GL_PROJECTION_MATRIX, P);
    float aspectRatio = window.GetWidth()/(float)window.GetHeight();
    mat4 P = glm::perspective(45.0f, aspectRatio, 0.01f, 1000.0f);
    mat4 V = glm::lookAt(camera->gameObject->transform->GetPosition(), camera->lookAt, camera->up);
    
    camera->ExtractVFPlanes(P, V);
    
    std::vector<Light> lights = setUpLights();
    
    for (GameObject *gameObject : world.GetGameObjects()) {
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
            applyCameraMatrix(shader, camera, world.mainCamera->transform->GetPosition());
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
            if (shader->hasUniform("lightPos")) glUniform3f(shader->getUniform("lightPos"), 5, 5, 5);
            if (shader->hasUniform("lightColor")) glUniform3f(shader->getUniform("lightColor"), 0, 0, 1);
            if (shader->hasUniform("sunDir")) glUniform3f(shader->getUniform("sunDir"), 0, 1, 0);
            if (shader->hasUniform("sunColor")) glUniform3f(shader->getUniform("sunColor"), 1, 1, 1);
            
            // all of this till next comment will be taken out
            
            if (shader->hasUniform("numLights")) glUniform1i(shader->getUniform("numLights"), lights.size());
            if (shader->hasUniform("allLights")) {
                for(int i = 0; i < lights.size(); ++i){
                    glUniform4f(shader->getUniform("position"), lights[i].position.x,lights[i].position.y,lights[i].position.z,lights[i].position.w);
                    glUniform3f(shader->getUniform("intensities"), lights[i].intensities.x,lights[i].intensities.y,lights[i].intensities.z);
                    glUniform1f(shader->getUniform("attenuation"), lights[i].attenuation);
                    glUniform1f(shader->getUniform("ambientCoefficient"), lights[i].ambientCoefficient);
                    glUniform1f(shader->getUniform("coneAngle"), lights[i].coneAngle);glUniform3f(shader->getUniform("coneDirection"), lights[i].coneDirection.x,lights[i].coneDirection.y,lights[i].coneDirection.z);
                }
            }
            if (shader->hasUniform("cameraPos")) glUniform3f(shader->getUniform("cameraPos"), world.mainCamera->transform->GetPosition().x,world.mainCamera->transform->GetPosition().y,world.mainCamera->transform->GetPosition().z);

            
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            applyProjectionMatrix(shader, window, camera);
            applyCameraMatrix(shader, camera, world.mainCamera->transform->GetPosition());
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
            if (shader->hasUniform("lightPos")) glUniform3f(shader->getUniform("lightPos"), 5, 5, 5);
            if (shader->hasUniform("lightColor")) glUniform3f(shader->getUniform("lightColor"), 1, 1, 1);
            if (shader->hasUniform("sunDir")) glUniform3f(shader->getUniform("sunDir"), 0, 1, 0);
            if (shader->hasUniform("sunColor")) glUniform3f(shader->getUniform("sunColor"), 1, 1, 1);
            
            Camera *camera = (Camera*)world.mainCamera->GetComponent("Camera");
            applyProjectionMatrix(shader, window, camera);
            applyCameraMatrix(shader, camera, world.mainCamera->transform->GetPosition());
            applyTransformMatrix(shader, gameObject->transform);
            
            terrain->draw(shader);
            
            shader->unbind();
        }
    }
}
