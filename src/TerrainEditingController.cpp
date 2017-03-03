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

#include "World.h"
#include "Components/TerrainRenderer.h"

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
    
}

void TerrainEditingController::MouseScrolled(World *world, double dx, double dy) {}
