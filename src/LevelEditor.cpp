//
//  LevelEditor.cpp
//  Shepherd
//
//  Created by Michael Cantrell on 3/4/17.
//
//

#include "LevelEditor.h"
#include "EntityFactory.h"
#include "Components/RigidBody.h"
#include "Components/Light.h"

void LevelEditor::drawLevelEditor(Window &window, World *world){
    ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
    ImGui::Begin("Object Creation Tool");
    static GameObject *mostRecentlyPlacedGameObject = nullptr;
    static int item = 0;
    static float position[3] = { 1.f, 1.f, 1.f };
    static float rotation[3] = { 0.f, 0.f, 0.f };
    static float scale = 1.f;
    static int type = 0;
    static bool isStatic = false;
    
    //for lights
    static bool isDirectional = false;
    static float intensities[3] = { 1.f, 1.f, 1.f };
    static float attenuation = 0.1;
    static float ambientCoefficient = 0.15;
    static float coneAngle = 360;
    static float coneDirection[3] = { 0.f, 1.f, 0.f };
    
    const char* items[] = {"Sphere", "Boulder", "Tree", "Light"};
    if (ImGui::Combo("Objects", &item, items, 4)) {
        
    }
    if (ImGui::Button("Add Item") && !mostRecentlyPlacedGameObject) {
        if(item == 0) {
            mostRecentlyPlacedGameObject = EntityFactory::createSphere(world, 2, vec3(0), 2);
        }
        if(item == 1) {
            type = rand() % 3;
            mostRecentlyPlacedGameObject = EntityFactory::createBoulder(world, type, 1, vec3(0));
        }
        if(item == 2) {
            type = (rand() % 2) + 1;
            mostRecentlyPlacedGameObject = EntityFactory::createTree(world, type, vec3(0));
        }
        if(item == 3) {
            mostRecentlyPlacedGameObject = EntityFactory::createLight(world, vec3(0), isDirectional, vec3(intensities[0],intensities[1],intensities[2]), attenuation, ambientCoefficient, coneAngle, vec3(coneDirection[0],coneDirection[1],coneDirection[2]));
        }
        //set transform
        mostRecentlyPlacedGameObject->transform->SetPosition(vec3(position[0],position[1],position[2]));
        mostRecentlyPlacedGameObject->transform->SetRotation(vec3(rotation[0],rotation[1],rotation[2]));
        mostRecentlyPlacedGameObject->transform->SetScale(glm::vec3(scale));
        //remove all physics
        RigidBody *rb = (RigidBody *)mostRecentlyPlacedGameObject->GetComponent("RigidBody");
        if(rb) {
            world->dynamicsWorld->removeRigidBody(rb->bulletRigidBody);
            rb->bulletRigidBody = nullptr;
        }
    }
    if(item == 3) {
        // Light menu stuff
        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
        ImGui::Begin("Light Creation Tool");
        
        if(ImGui::Checkbox("isDirectional", &isDirectional) && mostRecentlyPlacedGameObject) {
            Light *light = (Light *)mostRecentlyPlacedGameObject->GetComponent("Light");
            if(light) light->isDirectional = isDirectional;
        };
        if (ImGui::SliderFloat3("Intensities", intensities, 0, 1) && mostRecentlyPlacedGameObject) {
            Light *light = (Light *)mostRecentlyPlacedGameObject->GetComponent("Light");
            if(light) light->intensities = vec3(intensities[0],intensities[1],intensities[2]);
        };
        if (ImGui::SliderFloat("Attenuation", &attenuation, 0, 1.0) && mostRecentlyPlacedGameObject) {
            Light *light = (Light *)mostRecentlyPlacedGameObject->GetComponent("Light");
            if(light) light->attenuation = attenuation;
        };
        if (ImGui::SliderFloat("Cone Angle", &coneAngle, 0, 360) && mostRecentlyPlacedGameObject) {
            Light *light = (Light *)mostRecentlyPlacedGameObject->GetComponent("Light");
            if(light) light->coneAngle = coneAngle;
        };
        if (ImGui::SliderFloat3("Cone Direction", coneDirection, -1, 1) && mostRecentlyPlacedGameObject) {
            Light *light = (Light *)mostRecentlyPlacedGameObject->GetComponent("Light");
            if(light) light->coneDirection = vec3(coneDirection[0],coneDirection[1],coneDirection[2]);
        };
        
        ImGui::End();
    }
    if (mostRecentlyPlacedGameObject && ImGui::Button("Finalize Item")) {
        if(!isStatic) {
            // then add it to world without removing physics
            if(mostRecentlyPlacedGameObject->name == "Sphere") {
                world->RemoveGameObject(mostRecentlyPlacedGameObject);
                mostRecentlyPlacedGameObject = EntityFactory::createSphere(world, 2, vec3(position[0],position[1],position[2]), 2);
            }
            else if(mostRecentlyPlacedGameObject->name == "Boulder") {
                world->RemoveGameObject(mostRecentlyPlacedGameObject);
                mostRecentlyPlacedGameObject = EntityFactory::createBoulder(world, type, 1, vec3(position[0],position[1],position[2]));
            }
            else if(mostRecentlyPlacedGameObject->name == "Tree") {
                world->RemoveGameObject(mostRecentlyPlacedGameObject);
                mostRecentlyPlacedGameObject = EntityFactory::createTree(world, type, vec3(position[0],position[1],position[2]));
            }
            mostRecentlyPlacedGameObject->transform->SetPosition(vec3(position[0],position[1],position[2]));
            mostRecentlyPlacedGameObject->transform->SetRotation(vec3(rotation[0],rotation[1],rotation[2]));
            mostRecentlyPlacedGameObject->transform->SetScale(glm::vec3(scale));
        }
        mostRecentlyPlacedGameObject->SetIsStatic(isStatic);
        mostRecentlyPlacedGameObject = nullptr;
    }
    if (ImGui::Button("Get Current Position")) {
        GameObject *go = world->mainCamera;
        position[0] = go->transform->GetPosition().x;
        position[1] = go->transform->GetPosition().y;
        position[2] = go->transform->GetPosition().z;
    }
    if (ImGui::DragFloat3("Position", position)) {
        if(mostRecentlyPlacedGameObject != nullptr)
            mostRecentlyPlacedGameObject->transform->SetPosition(vec3(position[0],position[1],position[2]));
    }
    if (ImGui::DragFloat3("Rotation", rotation)) {
        if(mostRecentlyPlacedGameObject != nullptr)
            mostRecentlyPlacedGameObject->transform->SetRotation(vec3(rotation[0],rotation[1],rotation[2]));
    }
    if (ImGui::DragFloat("Scale", &scale)) {
        if(mostRecentlyPlacedGameObject != nullptr)
            mostRecentlyPlacedGameObject->transform->SetScale(vec3(scale));
    }
    if (ImGui::Checkbox("Static", &isStatic)) {
        
    }
    ImGui::End();
}
