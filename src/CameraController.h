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
    void MouseClicked(World *world, double mouseX, double mouseY, int key, int action);
	void MouseScrolled(World *world, double dx, double dy);
    void BeginShaking(int framesToShake, float intensity);
	void nextStage();
private:
//    float alpha = -M_PI/2;
//    float beta = -M_PI/2*3;
	glm::vec2 mousePrev = { 0.f, 0.f };
    int shakesRemaining = 0;
    float shakeIntensity = 0;
//	float rfactor = .01;
};

#endif /* CameraController_h */
