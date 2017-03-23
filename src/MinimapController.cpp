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
        ImGui::Begin("Terrain Heightmap2");
        ImGui::SetWindowPos(ImVec2(10, window->GetHeight()/2 - terrain->size - 47));
        ImGui::SetWindowSize(ImVec2(terrain->size + 18, terrain->size + 37));
        ImVec2 uv0 = ImVec2(0, 0);
        ImVec2 uv1 = ImVec2(1, 1);
        ImVec2 windowMin = ImGui::GetWindowContentRegionMin();
        
        // Draw terrain
        ImGui::Image((void*)(size_t)terrainRenderer->terrain->heightmapTex.texID, ImVec2(terrain->size, terrain->size), uv0, uv1, ImColor(255,255,255,100), ImColor(255,255,255,128));
        
        // Draw sheep and wolves
        for (GameObject *go : world->GetGameObjects()) {
            if (go->name.compare("Bunny") == 0) {
                float posX = (go->transform->GetPosition().x - terrainRenderer->gameObject->transform->GetPosition().x) / (terrainRenderer->gameObject->transform->GetScale().x * terrain->size);
                float posZ = (go->transform->GetPosition().z - terrainRenderer->gameObject->transform->GetPosition().z) / (terrainRenderer->gameObject->transform->GetScale().z * terrain->size);
                ImGui::SetCursorPos(ImVec2(windowMin.x + posX, windowMin.y + posZ));
                ImGui::Image((void*)(size_t)TextureLibrary::sheepIcon->texID, ImVec2(20, 20), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,0));
            } else if (go->name.compare("Wolf") == 0) {
                
            }
        }
        
        
        ImGui::SetCursorPos(ImVec2(windowMin.x + 70, windowMin.y));
        ImGui::Image((void*)(size_t)TextureLibrary::wolfIcon->texID, ImVec2(20, 20), uv0, uv1, ImColor(255,100,100,255), ImColor(255,255,255,0));
        
        // Draw goal
        ImGui::SetCursorPos(ImVec2(windowMin.x + terrain->size*4/5, windowMin.y + terrain->size*4/5));
        ImGui::Image((void*)(size_t)TextureLibrary::goalIcon->texID, ImVec2(20, 20), uv0, uv1, ImColor(255,255,0,255), ImColor(255,255,255,0));
        
        ImGui::End();
    }
}
