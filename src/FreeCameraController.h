//
//  FreeCameraController.h
//  Shepherd
//
//  Created by Tyler Dahl on 3/3/17.
//
//

#ifndef FreeCameraController_h
#define FreeCameraController_h

#include <cmath>

#include "WindowCallbackDelegate.h"

class FreeCameraController : WindowCallbackDelegate {
public:
    void Update(World &world);
    void KeyPressed(World *world, int windowWidth, int windowHeight, int key, int action);
    void MouseMoved(World *world, int windowWidth, int windowHeight, double mouseX, double mouseY);
    void MouseClicked(World *world, double mouseX, double mouseY, int key, int action);
    void MouseScrolled(World *world, double dx, double dy);
private:
    const float FAST_CAMERA_SPEED = 10.0f;
    const float SLOW_CAMERA_SPEED = 1.0f;
    const float STOP_THRESHOLD = 0.1;
    double alpha = -M_PI/2;
    double beta = -M_PI/2;
    float cameraSpeed = SLOW_CAMERA_SPEED;
};

#endif /* FreeCameraController_h */
