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
#include "ModelLibrary.h"
#include "ShaderLibrary.h"
#include "MaterialLibrary.h"
#include "PhysicsController.h"
#include "Time.h"
#include "Window.h"

Renderer::Renderer(Window &window) :window(window) {
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

void ApplyOrthoMatrix(Program *shader) {
    mat4 ortho = glm::ortho(-400.0f, 400.0f, -400.0f, 400.0f, 0.1f, 2000.0f);
    glUniformMatrix4fv(shader->getUniform("LP"), 1, GL_FALSE, value_ptr(ortho));
}

void SetLightView(Program *shader, vec3 pos, vec3 LA, vec3 up) {
    mat4 Cam = glm::lookAt(pos, LA, up);
    glUniformMatrix4fv(shader->getUniform("LV"), 1, GL_FALSE, value_ptr(Cam));
}

void applyCameraMatrix(Program *program, Camera *camera, glm::vec3 position) {
    MatrixStack stack = MatrixStack();
    stack.lookAt(position, camera->lookAt, camera->up);
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
    
    initShadows();
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

#define SHADOW_CASCADES 3
GLuint depthMapFBO;
GLuint depthMap[SHADOW_CASCADES];
mat4 shadowOrthos[SHADOW_CASCADES];

void calcShadowOrthos(World &world) {
    for (int i = 0; i < SHADOW_CASCADES; i++) {
        float magnitude = pow(2, (SHADOW_CASCADES-i-1)*2) * 40;
        float offsetX = 0.0f;
        float offsetZ = 0.0f;
        if (world.mainCharacter) {
            offsetX = world.mainCharacter->transform->GetPosition().x * cos(M_PI/4) - world.mainCharacter->transform->GetPosition().z * sin(M_PI/4);
            offsetZ = (-world.mainCharacter->transform->GetPosition().z * cos(M_PI/4) - world.mainCharacter->transform->GetPosition().x * sin(M_PI/4)) * cos(M_PI*55/180);;
        } else if (world.mainCamera) {
            offsetX = world.mainCamera->transform->GetPosition().x * cos(M_PI/4) - world.mainCamera->transform->GetPosition().z * sin(M_PI/4);
            offsetZ = (-world.mainCamera->transform->GetPosition().z * cos(M_PI/4) - world.mainCamera->transform->GetPosition().x * sin(M_PI/4)) * cos(M_PI*55/180);
        }
        shadowOrthos[i] = glm::ortho(-magnitude+offsetX, magnitude+offsetX, -magnitude+offsetZ, magnitude+offsetZ, 0.1f, 3000.0f);
    }
    shadowOrthos[0] = glm::ortho(-700.0f, 700.0f, -700.0f, 700.0f, 0.1f, 3000.0f);
}

void Renderer::initShadows() {
    int S_WIDTH = 2048;//window.GetWidth();
    int S_HEIGHT = 2048;//window.GetHeight();
    
    //generate the FBO for the shadow depth
    glGenFramebuffers(1, &depthMapFBO);
    
    //generate the texture
    glGenTextures(SHADOW_CASCADES, depthMap);
    for (int i = 0; i < SHADOW_CASCADES; i++) {
        glBindTexture(GL_TEXTURE_2D, depthMap[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, S_WIDTH, S_HEIGHT,
                     0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    }
    
    //bind with framebuffer's depth buffer
    glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
//    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[0], 0);
    for (int i = 0; i < SHADOW_CASCADES; i++) {
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[i], 0);
        glDrawBuffer(GL_NONE);
        glReadBuffer(GL_NONE);
    }
//    glDrawBuffer(GL_NONE);
//    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::RenderShadows(World &world) {
    calcShadowOrthos(world);
    
    //set up light's depth map
    glViewport(0, 0, 2048, 2048);//window.GetWidth(), window.GetHeight());
    
    
    glCullFace(GL_FRONT);
    for (int i = 0; i < SHADOW_CASCADES; i++) {
        // Render to shadow depth buffer 3
        glBindFramebuffer(GL_FRAMEBUFFER, depthMapFBO);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depthMap[i], 0);
        glClear(GL_DEPTH_BUFFER_BIT);
        //set up shadow shader
        //render scene
        Program *shadowShader = ShaderLibrary::shadowDepth->program;
        shadowShader->bind();
        glUniformMatrix4fv(shadowShader->getUniform("LP"), 1, GL_FALSE, value_ptr(shadowOrthos[i]));
        SetLightView(shadowShader, vec3(1000, 1000, 1000), vec3(0, 0, 0), vec3(0, 1, 0));
        //    drawScene(shadowShader, 0, 0);
        for (GameObject *gameObject : world.GetGameObjects()) {
            glUniform1i(shadowShader->getUniform("hasBones"), false);
            MeshRenderer *meshRenderer = (MeshRenderer*)gameObject->GetComponent("MeshRenderer");
            if (meshRenderer && meshRenderer->draw == false) continue;
            if (meshRenderer) {// && intersectFrustumAABB(camera, gameObject->getBounds().getMin(), gameObject->getBounds().getMax())) {
                
                if (shadowShader->hasUniform("Bones")) {
                    Animation* isAnim = (Animation*) gameObject->GetComponent("Animation");
                    if (isAnim) {
                        glUniform1i(shadowShader->getUniform("hasBones"), true);
                        glUniformMatrix4fv(shadowShader->getUniform("Bones"), //We find the location of the gBones uniform.
                                           //If the object is rigged...
                                           isAnim->skeleton.boneMats.size(),
                                           GL_FALSE,    //We don't need to transpose the matrices.
                                           &isAnim->skeleton.boneMats[0][0][0]);
                    }
                }
                
                if(gameObject->name.compare("Camera") == 0) {
                    glm::vec3 rot = gameObject->transform->GetRotation();
                    Transform t = *new Transform(gameObject->transform->GetPosition(), glm::vec3(rot.x, rot.y - 90, rot.z), gameObject->transform->GetScale());
                    applyTransformMatrix(shadowShader, &t);
                } else {
                    applyTransformMatrix(shadowShader, gameObject->transform);
                }
                meshRenderer->model->draw(shadowShader);
            }
            TerrainRenderer *terrainRenderer = (TerrainRenderer*)gameObject->GetComponent("TerrainRenderer");
            if (terrainRenderer) {
                applyTransformMatrix(shadowShader, gameObject->transform);
                terrainRenderer->terrain->draw();
            }
        }
        shadowShader->unbind();
    }
    glCullFace(GL_BACK);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::Render(World &world) {
    //#define DEBUG
#ifndef DEBUG
    
//    if (shadow) {
    RenderShadows(world);
//    }
    
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
    
    
//    std::vector<GameObject*> gos;
//    if (world.kdTree) {
//        gos = world.kdTree->getStaticObjectsInViewFrustrum(camera);
//    }
    
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
//            auto shader = terrainRenderer->shader->program;
            auto shader = ShaderLibrary::shadowTerrain->program;
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
            int i;
            for (i = 0; i < terrainRenderer->textures.size(); i++) {
                terrainRenderer->textures[i]->bind(i);
                glUniform1i(shader->getUniform(terrainRenderer->textures[i]->name), i);
            }
            
            /* set up light depth map for shadows */
            for (int j = 0; j < SHADOW_CASCADES; j++) {
                glActiveTexture(GL_TEXTURE0+i+j);
                glBindTexture(GL_TEXTURE_2D, depthMap[j]);
//                glUniform1i(shader->getUniform("shadowDepth"+to_string(j+1)), i+j);
                glUniformMatrix4fv(shader->getUniform("LP"+to_string(j+1)), 1, GL_FALSE, value_ptr(shadowOrthos[j]));
            }
            int texIDs[SHADOW_CASCADES] = {i, i+1, i+2};
            glUniform1iv(shader->getUniform("shadowDepth"), SHADOW_CASCADES, texIDs);
            glUniform3f(shader->getUniform("lightDir"), 1, 1, 1);
            
//            ApplyOrthoMatrix(shader);
            SetLightView(shader, vec3(1000, 1000, 1000), vec3(0, 0, 0), vec3(0, 1, 0));
            
            terrain->draw();
            
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


int Renderer::checkClickable(World &world, int mx, int my) {
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
    
    this->Render(world);
    return id;
}
