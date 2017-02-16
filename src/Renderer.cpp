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

bool intersectFrustumAABB(Camera *cam, vec3 boxMin, vec3 boxMax) {
    // Indexed for the 'index trick' later
    vec3 box[] = {boxMin, boxMax};
    
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

bool Renderer::ViewFrustCull(GameObject *gameObject, Camera *camera){
    
    float dist;
    Bounds b = gameObject->getBounds();
    
    vec3 boundPoints[8];
    vec3 boundPoint1 = boundPoints[0] = b.getMin();
    vec3 boundPoint2 = boundPoints[1] = b.getMax();
    vec3 boundPoint3 = boundPoints[2] = vec3(boundPoint1.x, boundPoint1.y, boundPoint2.z);
    vec3 boundPoint4 = boundPoints[3] = vec3(boundPoint1.x, boundPoint2.y, boundPoint1.z);
    vec3 boundPoint5 = boundPoints[4] = vec3(boundPoint2.x, boundPoint1.y, boundPoint1.z);
    vec3 boundPoint6 = boundPoints[5] = vec3(boundPoint1.x, boundPoint2.y, boundPoint2.z);
    vec3 boundPoint7 = boundPoints[6] = vec3(boundPoint2.x, boundPoint1.y, boundPoint2.z);
    vec3 boundPoint8 = boundPoints[7] = vec3(boundPoint2.x, boundPoint2.y, boundPoint1.z);

    return !intersectFrustumAABB(camera, b.getMin(), b.getMax());
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
    
    for (GameObject *gameObject : world.GetGameObjects()) {
        MeshRenderer *meshRenderer = (MeshRenderer*)gameObject->GetComponent("MeshRenderer");
        if (meshRenderer && !ViewFrustCull(gameObject, camera)) {
            auto shader = meshRenderer->shader->program;
            auto model = meshRenderer->model;
            shader->bind();
            
            if (meshRenderer->material) {
                applyMaterial(shader, meshRenderer->material);
            }
            if (shader->hasUniform("lightPos")) glUniform3f(shader->getUniform("lightPos"), 5, 5, 5);
            if (shader->hasUniform("lightColor")) glUniform3f(shader->getUniform("lightColor"), 1, 1, 1);
            if (shader->hasUniform("sunDir")) glUniform3f(shader->getUniform("sunDir"), 0, 1, 0);
            if (shader->hasUniform("sunColor")) glUniform3f(shader->getUniform("sunColor"), 1, 1, 1);
            if (shader->hasUniform("charge")) glUniform1f(shader->getUniform("charge"), 1.0);
            
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
