//
//  MinimapController.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 3/22/17.
//
//

#include "MinimapController.h"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Window.h"
#include "Components/TerrainRenderer.h"
#include "Terrain.h"
#include "TextureLibrary.h"

void MinimapController::ImguiUpdate(World *world, bool drawGUI) {
    if (!window || !terrainRenderer) return;
    Terrain *terrain = terrainRenderer->terrain;
    
    {
        ImGui::Begin("Terrain Heightmap2", nullptr, ImGuiWindowFlags_NoTitleBar|ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoMove|ImGuiWindowFlags_NoScrollbar|ImGuiWindowFlags_NoSavedSettings|ImGuiWindowFlags_NoInputs);
        ImGui::SetWindowPos(ImVec2(window->GetWidth()/2 - terrain->size - 28, window->GetHeight()/2 - terrain->size - 28));
        ImGui::SetWindowSize(ImVec2(terrain->size + 18, terrain->size + 18));
        ImVec2 uv0 = ImVec2(0, 0);
        ImVec2 uv1 = ImVec2(1, 1);
        ImVec2 windowMin = ImGui::GetWindowContentRegionMin();
        
        // Draw terrain
        ImGui::Image((void*)(size_t)terrainRenderer->terrain->heightmapTex.texID, ImVec2(terrain->size, terrain->size), uv0, uv1, ImColor(255,255,255,100), ImColor(255,255,255,128));
        
        glm::vec3 terrainSize = terrainRenderer->gameObject->transform->GetScale() * (float)terrain->size;
        glm::vec3 terrainMin = terrainRenderer->gameObject->transform->GetPosition() - terrainSize/2.0f;
        
        // Draw sheep and wolves
        for (GameObject *go : world->GetGameObjects()) {
            if (go->GetComponent("MeshRenderer")) {
                float posX = (go->transform->GetPosition().x - terrainMin.x) / terrainRenderer->gameObject->transform->GetScale().x;
                float posZ = (go->transform->GetPosition().z - terrainMin.z) / terrainRenderer->gameObject->transform->GetScale().z;
                ImGui::SetCursorPos(ImVec2(windowMin.x + posX, windowMin.y + posZ));
                if (go->name.compare("Bunny") == 0) {
                    ImGui::Image((void*)(size_t)TextureLibrary::sheepIcon->texID, ImVec2(20, 20), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,0));
                } else if (go->name.compare("Wolf") == 0) {
                    ImGui::Image((void*)(size_t)TextureLibrary::wolfIcon->texID, ImVec2(20, 20), uv0, uv1, ImColor(255,100,100,255), ImColor(255,255,255,0));
                }
            }
        }
        
        // Draw goal
        ImGui::SetCursorPos(ImVec2(windowMin.x + terrain->size*3/5, windowMin.y + terrain->size*3/4));
        ImGui::Image((void*)(size_t)TextureLibrary::goalIcon->texID, ImVec2(20, 20), uv0, uv1, ImColor(255,255,0,255), ImColor(255,255,255,0));
        
        if (world->mainCharacter) {
            float posX = (world->mainCharacter->transform->GetPosition().x - terrainMin.x) / terrainRenderer->gameObject->transform->GetScale().x;
            float posZ = (world->mainCharacter->transform->GetPosition().z - terrainMin.z) / terrainRenderer->gameObject->transform->GetScale().z;
            ImGui::SetCursorPos(ImVec2(windowMin.x + posX, windowMin.y + posZ));
            ImGui::Image((void*)(size_t)TextureLibrary::faceIcon->texID, ImVec2(15, 15), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,0));
        } else {
            float posX = (world->mainCamera->transform->GetPosition().x - terrainMin.x) / terrainRenderer->gameObject->transform->GetScale().x;
            float posZ = (world->mainCamera->transform->GetPosition().z - terrainMin.z) / terrainRenderer->gameObject->transform->GetScale().z;
            ImGui::SetCursorPos(ImVec2(windowMin.x + posX, windowMin.y + posZ));
            ImGui::Image((void*)(size_t)TextureLibrary::faceIcon->texID, ImVec2(15, 15), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,0));
        }
        
        ImGui::End();
    }
}
