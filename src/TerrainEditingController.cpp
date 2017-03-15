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
#include "TextureLoader.h"
#include "EntityFactory.h"

struct ToolProperties {
    int tool = 0;
    int radius = 10;
    float height = 20.0f;
    int kernel = 3;
    int kernelType = 0;
    int textureType = 0;
};

bool mousePressed = false;
NoiseProperties terrainProps;
ToolProperties toolProps;

void TerrainEditingController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
    // Re-generate Terrain
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_T) {
            if (terrainRenderer) {
                terrainRenderer->terrain->generate();
            }
        } else if (key == GLFW_KEY_SPACE) {
            if (!world->mainCharacter)
                world->mainCharacter = EntityFactory::upgradeCharacter(world, world->mainCamera, glm::vec3(60, 100, 40));
        }
    }
}

void TerrainEditingController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
    if (!mousePressed) return;
    
    return;
    
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
        
        std::cout << "Terrain edit position: (" << col << "," << row << ")" << std::endl;
        
        performAction(row, col);
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
        
        std::cout << "Terrain edit position: (" << col << "," << row << ")" << std::endl;
        
        performAction(row, col);
    }
}

void TerrainEditingController::MouseScrolled(World *world, double dx, double dy) {}

void TerrainEditingController::performAction(int row, int col) {
    Terrain *terrain = terrainRenderer->terrain;
    
    terrain->getHeight(col, row);
    if (toolProps.tool == 1) {
        elevate(col, row, toolProps.height, toolProps.radius);
    } else if (toolProps.tool == 2) {
        flatten(col, row, toolProps.height, toolProps.radius);
    } else if (toolProps.tool == 3) {
        smooth(col, row, toolProps.kernel, toolProps.radius, toolProps.kernelType == 0);
    } else if (toolProps.tool == 4) {
        smooth(col, row, toolProps.kernel, toolProps.radius, toolProps.kernelType == 0);
    }
    terrain->createMesh();
    terrain->update();
}

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
                terrain->setHeight(col, row, terrain->getHeight(col, row) + height);
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
                terrain->setHeight(col, row, height);
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
                                smoothedHeight += terrain->getHeight(col-kx, row-ky);
                            }
                        }
                    }
                    smoothedHeight /= count;
                    terrain->setHeight(col, row, smoothedHeight);
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
                                    smoothedHeight += terrain->getHeight(col-kx, row-ky);
                                }
                            }
                        }
                    }
                    smoothedHeight /= count;
                    terrain->setHeight(col, row, smoothedHeight);
                }
            }
        }
    }
}

void TerrainEditingController::texture(int x, int y, int radius) {
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
                terrain->setTexture(col, row, toolProps.textureType);
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
//    {
//        bool showTestWindow = true;
//        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
//        ImGui::ShowTestWindow(&showTestWindow);
//    }
    
    // Display terrain heightmap
    {
        ImGui::Begin("Terrain Heightmap");
        ImGui::SetWindowPos(ImVec2(10, 10));
        ImGui::SetWindowSize(ImVec2(terrain->size + 18, terrain->size + 37));
        ImVec2 uv0 = ImVec2(0, 0);
        ImVec2 uv1 = ImVec2(1, 1);
//        ImGui::Image((void*)terrain->getTexture()->getTextureId(), ImVec2(terrain->size, terrain->size), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
        ImGui::Image((void*)(size_t)terrainRenderer->terrain->heightmapTex.texID, ImVec2(terrain->size, terrain->size), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
        ImGui::End();
    }
    
    // Display terrain noise properties
    {
        ImGui::Begin("Terrain Settings");
        ImGui::SetWindowPos(ImVec2(10, terrain->size + 37 + 20));
        ImGui::SetWindowSize(ImVec2(terrain->size + 18, 350));
        
        if (ImGui::Button("New Seed")) {
            terrainProps.seed = time(0);
            terrain->generate(terrain->size, terrainProps);
            terrain->createMesh();
            terrain->update();
            EntityFactory::UpdateTerrain(world, terrainRenderer->gameObject, terrain);
        }
        ImGui::SameLine();
        ImGui::LabelText("", "%zu", terrainProps.seed ? terrainProps.seed : terrain->seed);
        
        if (ImGui::SliderFloat("Frequency", &terrainProps.frequency, 0.0f, 10.0f)) {
            std::cout << "Frequency: " << terrainProps.frequency << std::endl;
            terrain->generate(terrain->size, terrainProps);
        }
        if (ImGui::SliderInt("Octaves", &terrainProps.octaves, 0.0f, 10.0f)) {
            std::cout << "Octaves: " << terrainProps.octaves << std::endl;
            terrain->generate(terrain->size, terrainProps);
        }
        if (ImGui::SliderFloat("Octave Height", &terrainProps.octaveHeight, 0.0f, 100.0f)) {
            std::cout << "Octaves: " << terrainProps.octaveHeight << std::endl;
            terrain->generate(terrain->size, terrainProps);
        }
        
        ImGui::Separator();
        
        for (int i = 0; i < terrainRenderer->regionColors.size(); i++) {
            std::string heightStr = "Height" + to_string(i);
            std::string colorStr = "Color" + to_string(i);
            ImGui::SliderFloat(heightStr.c_str(), &terrainRenderer->regions[i], 0.0f, 1.0f);
            ImGui::ColorEdit3(colorStr.c_str(), &terrainRenderer->regionColors[i][0]);
        }
        
        if (ImGui::Button("Generate")) {
            terrain->generate(terrain->size, terrainProps);
            terrain->createMesh();
            terrain->update();
            EntityFactory::UpdateTerrain(world, terrainRenderer->gameObject, terrain);
        }
        ImGui::SameLine();
        if (ImGui::Button("Save")) {
            // Convert heightmap to unsigned chars
//            std::vector<unsigned char> heightmap;
//            std::vector<float> flattenedHeightmap = terrain->flattenHeightMap();
//            for (int i = 0; i < flattenedHeightmap.size(); i++) {
//                heightmap.push_back((unsigned char)flattenedHeightmap[i]);
//            }
//            
            // Save heightmap
//            if (!SOIL_save_image("Level_Heightmap.bmp", SOIL_SAVE_TYPE_BMP, terrain->size, terrain->size, 3, heightmap.data()))
//                std::cout << "Failed to save heightmap" << std::endl;
        }
        
        ImGui::End();
    }
    
    // Display terrain tool properties
    {
        ImGui::Begin("Tool Settings");
        
        static int tool = 0;
        if (ImGui::RadioButton("None", &tool, 0)) {
            toolProps.tool = 3;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Flatten", &tool, 2)) {
            toolProps.tool = 1;
        }
        if (ImGui::RadioButton("Elevate", &tool, 1)) {
            toolProps.tool = 0;
        }
        ImGui::SameLine();
        if (ImGui::RadioButton("Smooth", &tool, 3)) {
            toolProps.tool = 2;
        }
        if (ImGui::RadioButton("Texture", &tool, 4)) {
            toolProps.tool = 4;
        }
        ImGui::Separator();
        
        if (tool == 1) {
            ImGui::SliderInt("Radius", &toolProps.radius, 1, 100);
            ImGui::SliderFloat("Height", &toolProps.height, -100.0f, 100.0f);
        } else if (tool == 2) {
            ImGui::SliderInt("Radius", &toolProps.radius, 1, 100);
            ImGui::SliderFloat("Height", &toolProps.height, -100.0f, 100.0f);
        } else if (tool == 3) {
            ImGui::SliderInt("Radius", &toolProps.radius, 1, 100);
            ImGui::SliderInt("Kernel", &toolProps.kernel, 1, 25);
            ImGui::LabelText("Kernel Type", "Kernel Type");
            ImGui::RadioButton("Square", &toolProps.kernelType, 0);
            ImGui::SameLine();
            ImGui::RadioButton("Circle", &toolProps.kernelType, 1);
        } else if (tool == 4) {
            std::string item0 = "Grass";
            std::string item1 = "Rock";
            std::string item2 = "Snow";
            const char *items[] = {item0.c_str(), item1.c_str(), item2.c_str()};
            
//            ImGui::ImageButton(<#ImTextureID user_texture_id#>, <#const ImVec2 &size#>);
            ImGui::Combo("Textures", &toolProps.textureType, items, 3);
//            ImGui::Combo(<#const char *label#>, <#int *current_item#>, <#bool (*items_getter)(void *, int, const char **)#>, <#void *data#>, <#int items_count#>)
            
            ImGui::SliderInt("Radius", &toolProps.radius, 1, 100);
            ImGui::Checkbox("Use Painted Textures?", &terrain->useTextureMap);
        }
        
        ImGui::End();
    }
}
