//
//  Mesh.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Mesh_h
#define Mesh_h

#include <string>

#include "Bounds.h"
#include "../Shape.h"

class Mesh {
public:
    Mesh() {};
    Mesh(Shape *s) { shape = s; };
    virtual ~Mesh() {};
    
    Shape *shape = nullptr;
    
    static void LoadMeshes(std::string resourceDir);
    static Mesh *bunny;
    static Mesh *cube;
    static Mesh *plane;
};

#endif /* Mesh_h */
