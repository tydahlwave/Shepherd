//
//  Bounds.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#ifndef Bounds_h
#define Bounds_h

#include "glm/glm.hpp"
#include "Transform.h"

class Bounds {
public:
    Bounds() {};
    Bounds(glm::vec3 _min, glm::vec3 _max) {
        min = _min;
        max = _max;
        size = max - min;
        center = (_min + _max) / glm::vec3(2, 2, 2);
    };
    Bounds(glm::vec3 c, glm::vec3 _min, glm::vec3 _max, glm::vec3 s) {
        center = c;
        min = _min;
        max = _max;
        size = s;
    }
    virtual ~Bounds() {};

    glm::vec3 center = glm::vec3(0, 0, 0);
    glm::vec3 min = glm::vec3(-1, -1, -1);
    glm::vec3 max = glm::vec3(1, 1, 1);
    glm::vec3 size = glm::vec3(2, 2, 2);
    
    Bounds *TransformedBounds(Transform *transform) {
        const glm::mat4 &transformMatrix = transform->GetMatrix();
        
        vec4 corners[8];
        corners[0] = transformMatrix * vec4(min[0], min[1], min[2], 1);
        corners[1] = transformMatrix * vec4(min[0], min[1], max[2], 1);
        corners[2] = transformMatrix * vec4(min[0], max[1], min[2], 1);
        corners[3] = transformMatrix * vec4(min[0], max[1], max[2], 1);
        corners[4] = transformMatrix * vec4(max[0], min[1], min[2], 1);
        corners[5] = transformMatrix * vec4(max[0], min[1], max[2], 1);
        corners[6] = transformMatrix * vec4(max[0], max[1], min[2], 1);
        corners[7] = transformMatrix * vec4(max[0], max[1], max[2], 1);
        vec4 newMin(corners[0].x, corners[0].y, corners[0].z, corners[0].w);
        vec4 newMax(corners[0].x, corners[0].y, corners[0].z, corners[0].w);
    
        // Recompute max and min
        for (int i = 0; i < 3; i++) {
            for (int j = 1; j < 8; j++) {
                if (corners[j][i] < newMin[i]) {
                    newMin[i] = corners[j][i];
                }
                if (corners[j][i] > newMax[i]) {
                    newMax[i] = corners[j][i];
                }
            }
        }
        
        return new Bounds(newMin, newMax);
    }
    
    bool Intersects(Bounds *other) {
        return (min.x <= other->max.x && max.x >= other->min.x) &&
               (min.y <= other->max.y && max.y >= other->min.y) &&
               (min.z <= other->max.z && max.z >= other->min.z);
    }
    
    Bounds *Intersection(Bounds *other) {
        if (Intersects(other)) {
            float minX = (min.x > other->min.x) ? min.x : other->min.x;
            float minY = (min.y > other->min.y) ? min.y : other->min.y;
            float minZ = (min.z > other->min.z) ? min.z : other->min.z;
            float maxX = (max.x < other->max.x) ? max.x : other->max.x;
            float maxY = (max.y < other->max.y) ? max.y : other->max.y;
            float maxZ = (max.z < other->max.z) ? max.z : other->max.z;
            return new Bounds(glm::vec3(minX, minY, minZ), glm::vec3(maxX, maxY, maxZ));
        }
        return nullptr;
    }
};

#endif /* Bounds_h */
