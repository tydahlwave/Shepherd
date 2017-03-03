//
//  TerrainEditingController.h
//  Shepherd
//
//  Created by Tyler Dahl on 3/3/17.
//
//

#ifndef TerrainEditingController_h
#define TerrainEditingController_h

#include "WindowCallbackDelegate.h"

class TerrainRenderer;

class TerrainEditingController : WindowCallbackDelegate {
public:
    void SetTerrain(TerrainRenderer *terrainRenderer) { this->terrainRenderer = terrainRenderer; };
    
    void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action);
    void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY);
    void MouseClicked(World *world, double mouseX, double mouseY, int key, int action);
    void MouseScrolled(World *world, double dx, double dy);
private:
    TerrainRenderer *terrainRenderer;
};

#endif /* TerrainEditingController_h */
