//
//  TerrainController.h
//  Shepherd
//
//  Created by Tyler Dahl on 2/3/17.
//
//

#ifndef TerrainController_h
#define TerrainController_h

#include "WindowCallbackDelegate.h"

class World;

class TerrainController : WindowCallbackDelegate {
public:
    TerrainController() {};
    virtual ~TerrainController() {};
    
    void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action);
    void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY);
    void MouseClicked(World *world, double mouseX, double mouseY, int key, int action);
	void MouseScrolled(World *world, double dx, double dy);

};

#endif /* TerrainController_h */
