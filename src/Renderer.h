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

/*
 Represents a point light
 w == 0 in the position indications a directional light
 */
struct Light {
    glm::vec4 position;
    glm::vec3 intensities; //a.k.a. the color of the light
    float attenuation;
    float ambientCoefficient;
    float coneAngle;
    glm::vec3 coneDirection;
};

class Renderer {
public:
    Renderer();
    virtual ~Renderer() {};
    void Initialize();
    void Render(World &world, Window &window);
};

#endif /* Renderer_h */
