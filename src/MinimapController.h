//
//  MinimapController.h
//  Shepherd
//
//  Created by Tyler Dahl on 3/22/17.
//
//

#ifndef MinimapController_h
#define MinimapController_h

#include "ImguiUpdateDelegate.h"

class TerrainRenderer;
class Window;

class MinimapController: ImguiUpdateDelegate {
public:
    void SetTerrain(TerrainRenderer *terrainRenderer) { this->terrainRenderer = terrainRenderer; };
    void SetWindow(Window *window) { this->window = window; };
    
    void ImguiUpdate(World *world, bool drawGUI) override;
private:
    TerrainRenderer *terrainRenderer;
    Window *window;
};

#endif /* MinimapController_h */
