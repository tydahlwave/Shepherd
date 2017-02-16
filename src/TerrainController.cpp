//
//  TerrainController.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 2/3/17.
//
//

#include "TerrainController.h"

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "World.h"
#include "Components/TerrainRenderer.h"

void TerrainController::KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action) {
    // Re-generated Terrain
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_SPACE) {
            for (GameObject *go : world->GetGameObjects()) {
                if (go->name.compare("Terrain") == 0) {
                    TerrainRenderer *terrainRenderer = (TerrainRenderer*) go->GetComponent("TerrainRenderer");
                    terrainRenderer->terrain->Regenerate();
                }
            }
        }
    }
}

void TerrainController::MouseClicked(World *world, double mouseX, double mouseY, int key, int action) {
    
}

void TerrainController::MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY) {
    
}


void TerrainController::MouseScrolled(World *world, double dx, double dy) {

}
