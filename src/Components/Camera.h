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
    Camera(ProjectionType p, float n, float f, float _fov, glm::vec3 l, glm::vec3 u, glm::vec3 o) :Camera()
    { projectionType = p; near = n; far = f; fov = _fov; lookAt = l; up = u; }
    virtual ~Camera() {};
    
    ProjectionType projectionType = Perspective;
    float near = 1;
    float far = 1000;
    float fov = 45.0f; // field of view
    glm::vec3 lookAt = glm::vec3(0, 0, -1);
    glm::vec3 up = glm::vec3(0, 1, 0);
    
    vec4 Left, Right, Bottom, Top, Near, Far;
    vec4 planes[6];
    
    void ExtractVFPlanes(mat4 P, mat4 V) {
        
        /* composite matrix */
        mat4 comp = P*V;
        vec3 n; //use to pull out normal
        float l; //length of normal for plane normalization
        
        Left.x = comp[0][3] + comp[0][0]; // see handout to fill in with values from comp
        Left.y = comp[1][3] + comp[1][0]; // see handout to fill in with values from comp
        Left.z = comp[2][3] + comp[2][0]; // see handout to fill in with values from comp
        Left.w = comp[3][3] + comp[3][0]; // see handout to fill in with values from comp
        n = vec3(Left.x,Left.y,Left.z);
        l = glm::length(n);
        planes[0] = Left = Left/l;
        
        Right.x = comp[0][3] - comp[0][0]; // see handout to fill in with values from comp
        Right.y = comp[1][3] - comp[1][0]; // see handout to fill in with values from comp
        Right.z = comp[2][3] - comp[2][0]; // see handout to fill in with values from comp
        Right.w = comp[3][3] - comp[3][0]; // see handout to fill in with values from comp
        n = vec3(Right.x,Right.y,Right.z);
        l = glm::length(n);
        planes[1] = Right = Right/l;
        
        Bottom.x = comp[0][3] + comp[0][1]; // see handout to fill in with values from comp
        Bottom.y = comp[1][3] + comp[1][1]; // see handout to fill in with values from comp
        Bottom.z = comp[2][3] + comp[2][1]; // see handout to fill in with values from comp
        Bottom.w = comp[3][3] + comp[3][1]; // see handout to fill in with values from comp
        n = vec3(Bottom.x,Bottom.y,Bottom.z);
        l = glm::length(n);
        planes[2] = Bottom = Bottom/l;
        
        Top.x = comp[0][3] - comp[0][1]; // see handout to fill in with values from comp
        Top.y = comp[1][3] - comp[1][1]; // see handout to fill in with values from comp
        Top.z = comp[2][3] - comp[2][1]; // see handout to fill in with values from comp
        Top.w = comp[3][3] - comp[3][1]; // see handout to fill in with values from comp
        n = vec3(Top.x,Top.y,Top.z);
        l = glm::length(n);
        planes[3] = Top = Top/l;
        
        Near.x = comp[0][3] + comp[0][2]; // see handout to fill in with values from comp
        Near.y = comp[1][3] + comp[1][2]; // see handout to fill in with values from comp
        Near.z = comp[2][3] + comp[2][2]; // see handout to fill in with values from comp
        Near.w = comp[3][3] + comp[3][2]; // see handout to fill in with values from comp
        n = vec3(Near.x,Near.y,Near.z);
        l = glm::length(n);
        planes[4] = Near = Near/l;
        
        Far.x = comp[0][3] - comp[0][2]; // see handout to fill in with values from comp
        Far.y = comp[1][3] - comp[1][2]; // see handout to fill in with values from comp
        Far.z = comp[2][3] - comp[2][2]; // see handout to fill in with values from comp
        Far.w = comp[3][3] - comp[3][2]; // see handout to fill in with values from comp
        n = vec3(Far.x,Far.y,Far.z);
        l = glm::length(n);
        planes[5] = Far = Far/l;
    }
	glm::vec3 vel = glm::vec3(0, 0, 0);
	float dist = 10;
	float aap = 0;
	float pitch = 0;
	float yaw = 0;
	float roll = 0;
	glm::vec3 pos = glm::vec3(0, 0, 0);
};

#endif /* Camera_h */
