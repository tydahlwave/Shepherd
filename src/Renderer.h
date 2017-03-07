//
//  Renderer.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Renderer_h
#define Renderer_h

#include "World.h"
#include "Window.h"
#include "Components/Camera.h"

class Renderer {
public:
    Renderer();
    virtual ~Renderer() {};
    void Initialize();
	void Render(World &world, Window &window);
	int checkClickable(World &world, Window &window, int mx, int my);
};

#endif /* Renderer_h */
