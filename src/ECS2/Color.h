//
//  Color.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Color_h
#define Color_h

#include "glm/glm.hpp"

class Color {
public:
    Color() {};
    Color(glm::vec4 v) { value = v; }
    Color(float r, float g, float b, float a) { value = glm::vec4(r, g, b, a); }
    virtual ~Color() {};
    
    glm::vec4 value = glm::vec4(1, 1, 1, 1); // r, g, b, a
};

#endif /* Color_h */
