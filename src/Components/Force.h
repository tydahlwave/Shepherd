//
//  Force.h
//  Shepherd
//
//  Created by Tyler Dahl on 3/16/17.
//
//

#ifndef Force_h
#define Force_h

#include "glm/glm.hpp"

#include "Component.h"

class Force : public Component {
public:
    glm::vec3 dir = glm::vec3(0);
    long time = 0;
    
    Force() { name = "Force"; };
    virtual ~Force() {};
    
};

#endif /* Force_h */
