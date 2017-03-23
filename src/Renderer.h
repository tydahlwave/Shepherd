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
    Renderer(Window &window);
    virtual ~Renderer() {};
    
    void Initialize();
    void initShadows();
    
	void Render(World &world);
    void RenderShadows(World &world);
    
	int checkClickable(World &world, int mx, int my);
    static bool intersectFrustumAABB(Camera *cam, vec3 min, vec3 max);

private:
	float moveFactor = 0;
	float waveSpeed = 0.03f;
    Window &window;

};

#endif /* Renderer_h */
