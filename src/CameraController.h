//
//  CameraController.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#ifndef CameraController_h
#define CameraController_h

#include <cmath>

#include "World.h"
#include "WindowCallbackDelegate.h"

class CameraController : WindowCallbackDelegate {
public:
    CameraController() {};
    virtual ~CameraController() {};
    
    void Update(World &world);
    void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action);
    void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY);
private:
    float alpha = -M_PI/2;
    float beta = -M_PI/2*3;
    float cameraSpeed = 0.2;
    float cameraStoppedThreshold = 0.1;
};

#endif /* CameraController_h */
