//
//  PhysicsController.h
//  SimpleGame
//
//  Created by Michael Cantrell on 1/29/17.
//
//

#ifndef PhysicsController_h
#define PhysicsController_h

#include <cmath>

#include "World.h"
#include "WindowCallbackDelegate.h"

class PhysicsController : WindowCallbackDelegate {
public:
    PhysicsController() {};
    virtual ~PhysicsController() {};
    
    void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action);
    void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY);
    void MouseClicked(World *world, double mouseX, double mouseY, int key, int action);
    bool isLyingInCone(btVector3 &x, btVector3 &t, btVector3 &b, float aperture);
    
    long LeftClickPressTime;
    long RightClickPressTime;
};

#endif /* CameraController_h */
