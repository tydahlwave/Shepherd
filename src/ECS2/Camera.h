//
//  Camera.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Camera_h
#define Camera_h

#include "Component.h"

enum ProjectionType {
    Orthographic,
    Perspective
};

class Camera : public Component {
public:
    Camera() { name = "Camera"; };
    Camera(ProjectionType p, float n, float f, float _fov, glm::vec3 l, glm::vec3 u) :Camera()
    { projectionType = p; near = n; far = f; fov = _fov; lookAt = l; up = u; }
    virtual ~Camera() {};
    
    ProjectionType projectionType = Perspective;
    float near = 1;
    float far = 1000;
    float fov = 45.0f; // field of view
    glm::vec3 lookAt = glm::vec3(0, 0, -1);
    glm::vec3 up = glm::vec3(0, 1, 0);
};

#endif /* Camera_h */
