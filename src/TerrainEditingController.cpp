//
//  TerrainEditingController.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 3/3/17.
//
//

#include "TerrainEditingController.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Noise/NoiseProperties.h"

#include "World.h"
#include "Components/TerrainRenderer.h"

bool heightmapNeedsUpdate = false;
NoiseProperties terrainProps;

void TerrainEditingController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
    // Re-generate Terrain
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_T) {
            if (terrainRenderer) {
                terrainRenderer->terrain->Regenerate();
            }
        }
    }
}

void TerrainEditingController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {}

void TerrainEditingController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {
    if (action != GLFW_PRESS) return;
    
    Terrain *terrain = terrainRenderer->terrain;
    Camera *camera = (Camera*)world->mainCamera->GetComponent("Camera");
    glm::vec3 position = world->mainCamera->transform->GetPosition();
    glm::vec3 direction = glm::normalize(camera->lookAt - position);
    float planeA = 0.0f;
    float planeB = 1.0f;
    float planeC = 0.0f;
    float planeD = 0.0f;
    
    // Plane normal dot direction
    float denominator = (planeA*direction.x + planeB*direction.y + planeC*direction.z);
    if (denominator != 0) {
        float numerator = -(planeA*position.x + planeB*position.y + planeC*position.z + planeD);
        float intersectionTime = numerator / denominator;
        
        // Get the location of the intersection
        glm::vec3 hit = position + direction * intersectionTime;
        
        // Get the heightmap coords of the intersection
        int row = (int)hit.x + terrain->size/2;
        int col = (int)hit.z + terrain->size/2;
        
        std::cout << "Change height: (" << col << "," << row << ")" << std::endl;
        
        terrain->getHeight(row, col);
//        elevate(col, row, 20, 5);
        flatten(col, row, 0, 5);
        terrain->UpdateBuffers();
        terrain->update();
//        terrain->makeTexture();
//        heightmapNeedsUpdate = true;
    }
}

void TerrainEditingController::MouseScrolled(World *world, double dx, double dy) {}

void TerrainEditingController::elevate(int x, int y, float height, int radius) {
    Terrain *terrain = terrainRenderer->terrain;
    int radiusSquared = radius * radius;
    
    // For every row within bounds
    for (int row = y - radius; row < y + radius; row++) {
        if (row < 0 || row >= terrain->size) continue;
        
        // For every column within bounds
        for (int col = x - radius; col < x + radius; col++) {
            if (col < 0 || col >= terrain->size) continue;
            
            // If this (row,col) is within radius
            if (pow(row-y, 2) + pow(col-x, 2) < radiusSquared) {
                terrain->heightMap[row][col] += height;
            }
        }
    }
}

void TerrainEditingController::flatten(int x, int y, float height, int radius) {
    Terrain *terrain = terrainRenderer->terrain;
    int radiusSquared = radius * radius;
    
    // For every row within bounds
    for (int row = y - radius; row < y + radius; row++) {
        if (row < 0 || row >= terrain->size) continue;
        
        // For every column within bounds
        for (int col = x - radius; col < x + radius; col++) {
            if (col < 0 || col >= terrain->size) continue;
            
            // If this (row,col) is within radius
            if (pow(row-y, 2) + pow(col-x, 2) < radiusSquared) {
                terrain->heightMap[row][col] = height;
            }
        }
    }
}

void TerrainEditingController::ImguiUpdate(World *world) {
    Terrain *terrain = terrainRenderer->terrain;
    
    // 1. Show a simple window
    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
    {
        ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiSetCond_FirstUseEver);
        ImGui::SetNextWindowContentSize(ImVec2(100, 20));
        ImGui::Begin("Debug");
        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
        ImGui::End();
    }
    
    // 2. Show another simple window, this time using an explicit Begin/End pair
    //    if (show_another_window)
    //    {
    //        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
    //        ImGui::Begin("Another Window", &show_another_window);
    //        ImGui::Text("Hello");
    //        ImGui::End();
    //    }
    
    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
    //    if (show_test_window)
    //    {
    //        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
    //        ImGui::ShowTestWindow(&show_test_window);
    //    }
    
    // Display terrain heightmap
    {
        ImGui::Begin("Terrain Heightmap");
        ImGui::SetWindowPos(ImVec2(10, 10));
        ImGui::SetWindowSize(ImVec2(terrain->size + 18, terrain->size + 37));
        ImVec2 uv0 = ImVec2(0, 0);
        ImVec2 uv1 = ImVec2(1, 1);
        ImGui::Image((void*)terrain->getTexture()->getTextureId(), ImVec2(terrain->size, terrain->size), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
        ImGui::End();
    }
    
    {
        ImGui::Begin("Terrain Settings");
        ImGui::SetWindowPos(ImVec2(10, terrain->size + 37 + 20));
        ImGui::SetWindowSize(ImVec2(terrain->size + 18, 350));
        
        if (ImGui::Button("New Seed")) {
            terrainProps.seed = time(0);
            terrain->GenerateHeightmap(terrainProps, terrain->seed);
            terrain->UpdateBuffers();
            terrain->update();
        }
        ImGui::LabelText("Seed", "%zu", terrainProps.seed ? terrainProps.seed : terrain->seed);
        
        if (ImGui::SliderFloat("Frequency", &terrainProps.frequency, 0.0f, 10.0f)) {
            std::cout << "Frequency: " << terrainProps.frequency << std::endl;
            terrain->GenerateHeightmap(terrainProps, terrain->seed);
            terrain->makeTexture();
        }
        
        if (ImGui::SliderInt("Octaves", &terrainProps.octaves, 0.0f, 10.0f)) {
            std::cout << "Octaves: " << terrainProps.octaves << std::endl;
            terrain->GenerateHeightmap(terrainProps, terrain->seed);
            terrain->makeTexture();
        }
        
        for (int i = 0; i < terrainRenderer->regionColors.size(); i++) {
            std::string heightStr = "Height" + to_string(i);
            std::string colorStr = "Color" + to_string(i);
            ImGui::SliderFloat(heightStr.c_str(), &terrainRenderer->regions[i], 0.0f, 1.0f);
            ImGui::DragFloat3(colorStr.c_str(), &terrainRenderer->regionColors[i][0], 0.02f, 0.0f, 1.0f);
        }
        
        if (ImGui::Button("Generate")) {
            terrain->GenerateHeightmap(terrainProps, terrain->seed);
            terrain->UpdateBuffers();
            terrain->update();
        }
        
        ImGui::End();
    }
}
