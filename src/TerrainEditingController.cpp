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

#include "SOIL/SOIL.h"

#include "Noise/NoiseProperties.h"

#include "World.h"
#include "Components/TerrainRenderer.h"
#include "TextureLoader.h"
#include "EntityFactory.h"

struct ToolProperties {
    int tool = 0;
    int radius = 10;
    float height = 20.0f;
    int kernel = 3;
    int kernelType = 0;
};

bool mousePressed = false;
NoiseProperties terrainProps;
ToolProperties toolProps;

void TerrainEditingController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
    // Re-generate Terrain
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_T) {
            if (terrainRenderer) {
                terrainRenderer->terrain->Regenerate();
            }
        } else if (key == GLFW_KEY_SPACE) {
            if (!world->mainCharacter)
                world->mainCharacter = EntityFactory::upgradeCharacter(world, world->mainCamera);
        }
    }
}

void TerrainEditingController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
    if (!mousePressed) return;
    
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
        if (toolProps.tool == 0) {
            elevate(col, row, toolProps.height, toolProps.radius);
        } else if (toolProps.tool == 1) {
            flatten(col, row, toolProps.height, toolProps.radius);
        } else if (toolProps.tool == 2) {
            smooth(col, row, toolProps.kernel, toolProps.radius, toolProps.kernelType == 0);
        }
        terrain->UpdateBuffers();
        terrain->update();
        //        terrain->makeTexture();
    }
}

void TerrainEditingController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {
    mousePressed = (action == GLFW_PRESS);
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
        if (toolProps.tool == 0) {
            elevate(col, row, toolProps.height, toolProps.radius);
        } else if (toolProps.tool == 1) {
            flatten(col, row, toolProps.height, toolProps.radius);
        } else if (toolProps.tool == 2) {
            smooth(col, row, toolProps.kernel, toolProps.radius, toolProps.kernelType == 0);
        }
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

void TerrainEditingController::smooth(int x, int y, int kernel, int radius, bool square) {
    Terrain *terrain = terrainRenderer->terrain;
    int radiusSquared = radius * radius;
    int kernelSquared = kernel*kernel;
    
    // For every row within bounds
    for (int row = y - radius; row < y + radius; row++) {
        if (row < 0 || row >= terrain->size) continue;
        
        // For every column within bounds
        for (int col = x - radius; col < x + radius; col++) {
            if (col < 0 || col >= terrain->size) continue;
            
            // If this (row,col) is within radius
            if (pow(row-y, 2) + pow(col-x, 2) < radiusSquared) {
                
                bool square = true;
                if (square) {
                    int count = 0;
                    float smoothedHeight = 0;
                    for (int ky = -kernel; ky < kernel; ky++) {
                        for (int kx = -kernel; kx < kernel; kx++) {
                            if (row-ky >= 0 && row-ky < terrain->size && col-kx >= 0 && col-kx < terrain->size) {
                                count++;
                                smoothedHeight += terrain->heightMap[row-ky][col-kx];
                            }
                        }
                    }
                    smoothedHeight /= count;
                    terrain->heightMap[row][col] = smoothedHeight;
                } else {
                    int count = 0;
//                    int sum = 0;
                    float smoothedHeight = 0;
                    for (int ky = -kernel; ky < kernel; ky++) {
                        for (int kx = -kernel; kx < kernel; kx++) {
                            if (row-ky >= 0 && row-ky < terrain->size && col-kx >= 0 && col-kx < terrain->size) {
                                // If within the kernel radius
                                if (pow(row-y, 2) + pow(col-x, 2) < kernelSquared) {
                                    count++;
                                    smoothedHeight += terrain->heightMap[row-ky][col-kx];
                                }
                            }
                        }
                    }
                    smoothedHeight /= count;
                    terrain->heightMap[row][col] = smoothedHeight;
                }
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
//        ImGui::Image((void*)terrain->getTexture()->getTextureId(), ImVec2(terrain->size, terrain->size), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
        ImGui::Image((void*)(size_t)terrainRenderer->terrain->getTexture()->getTextureId(), ImVec2(terrain->size, terrain->size), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
        ImGui::End();
    }
    
    // Display terrain noise properties
    {
        ImGui::Begin("Terrain Settings");
        ImGui::SetWindowPos(ImVec2(10, terrain->size + 37 + 20));
        ImGui::SetWindowSize(ImVec2(terrain->size + 18, 350));
        
        if (ImGui::Button("New Seed")) {
            terrainProps.seed = time(0);
            terrain->GenerateHeightmap(terrainProps, terrain->seed);
            terrain->UpdateBuffers();
            terrain->update();
            EntityFactory::UpdateTerrain(world, terrainRenderer->gameObject, terrain);
        }
        ImGui::SameLine();
        ImGui::LabelText("", "%zu", terrainProps.seed ? terrainProps.seed : terrain->seed);
        
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
        if (ImGui::SliderFloat("Octave Height", &terrainProps.octaveHeight, 0.0f, 100.0f)) {
            std::cout << "Octaves: " << terrainProps.octaveHeight << std::endl;
            terrain->GenerateHeightmap(terrainProps, terrain->seed);
            terrain->makeTexture();
        }
        
        ImGui::Separator();
        
        for (int i = 0; i < terrainRenderer->regionColors.size(); i++) {
            std::string heightStr = "Height" + to_string(i);
            std::string colorStr = "Color" + to_string(i);
            ImGui::SliderFloat(heightStr.c_str(), &terrainRenderer->regions[i], 0.0f, 1.0f);
            ImGui::ColorEdit3(colorStr.c_str(), &terrainRenderer->regionColors[i][0]);
        }
        
        if (ImGui::Button("Generate")) {
            terrain->GenerateHeightmap(terrainProps, terrain->seed);
            terrain->UpdateBuffers();
            terrain->update();
            EntityFactory::UpdateTerrain(world, terrainRenderer->gameObject, terrain);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            // Convert heightmap to unsigned chars
            std::vector<unsigned char> heightmap;
            std::vector<float> flattenedHeightmap = terrain->flattenHeightMap();
            for (int i = 0; i < flattenedHeightmap.size(); i++) {
                heightmap.push_back((unsigned char)flattenedHeightmap[i]);
            }
            
            // Save heightmap
            if (!SOIL_save_image("Level_Heightmap.bmp", SOIL_SAVE_TYPE_BMP, terrain->size, terrain->size, 3, heightmap.data()))
                std::cout << "Failed to save heightmap" << std::endl;
        }
        
        ImGui::End();
    }
    
    // Display terrain tool properties
    {
        ImGui::Begin("Tool Settings");
        
        static int tool = 0;
        if (ImGui::RadioButton("Elevate", &tool, 0)) {
            toolProps.tool = 0;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Smooth", &tool, 2)) {
            toolProps.tool = 2;
        }
        if (ImGui::RadioButton("Flatten", &tool, 1)) {
            toolProps.tool = 1;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("None", &tool, 3)) {
            toolProps.tool = 3;
        }
        ImGui::Separator();
        
        if (tool == 0) {
            ImGui::SliderInt("Radius", &toolProps.radius, 1, 25);
            ImGui::SliderFloat("Height", &toolProps.height, -100.0f, 100.0f);
        } else if (tool == 1) {
            ImGui::SliderInt("Radius", &toolProps.radius, 1, 25);
            ImGui::SliderFloat("Height", &toolProps.height, -100.0f, 100.0f);
        } else if (tool == 2) {
            ImGui::SliderInt("Radius", &toolProps.radius, 1, 25);
            ImGui::SliderInt("Kernel", &toolProps.kernel, 1, 25);
            ImGui::LabelText("Kernel Type", "Kernel Type");
            ImGui::RadioButton("Square", &toolProps.kernelType, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Circle", &toolProps.kernelType, 1);
        }
        
        ImGui::End();
    }
}