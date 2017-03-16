//
//  TerrainEditingController.h
//  Shepherd
//
//  Created by Tyler Dahl on 3/3/17.
//
//

#ifndef TerrainEditingController_h
#define TerrainEditingController_h

#include <string>

#include "WindowCallbackDelegate.h"
#include "ImguiUpdateDelegate.h"

class TerrainRenderer;

class TerrainEditingController : WindowCallbackDelegate, ImguiUpdateDelegate {
public:
    void SetTerrain(TerrainRenderer *terrainRenderer) { this->terrainRenderer = terrainRenderer; };
    void SetResourceDir(std::string resourceDir) { this->resourceDir = resourceDir; };
    
    void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action);
    void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY);
    void MouseClicked(World *world, double mouseX, double mouseY, int key, int action);
    void MouseScrolled(World *world, double dx, double dy);
    void ImguiUpdate(World *world, bool drawGUI);
private:
    TerrainRenderer *terrainRenderer;
    std::string resourceDir;
    
    void performAction(int row, int col);
    void elevate(int row, int col, float height, int radius);
    void flatten(int x, int y, float height, int radius);
    void smooth(int x, int y, int kernel, int radius, bool square);
    void texture(int x, int y, int radius);
};

#endif /* TerrainEditingController_h */
