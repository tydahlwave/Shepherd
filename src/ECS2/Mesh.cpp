//
//  Mesh.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#include "Mesh.h"

Mesh *Mesh::bunny = nullptr;
Mesh *Mesh::cube = nullptr;
Mesh *Mesh::plane = nullptr;

void Mesh::LoadMeshes(std::string resourceDir) {
    Shape *shape = new Shape();
    shape->loadMesh(resourceDir + "bunny.obj");
    shape->resize();
    shape->init();
    bunny = new Mesh(shape);
    
    shape = new Shape();
    shape->loadMesh(resourceDir + "cube.obj");
    shape->resize();
    shape->init();
    cube = new Mesh(shape);
    
    shape = new Shape();
    shape->loadMesh(resourceDir + "plane.obj");
    shape->resize();
    shape->init();
    plane = new Mesh(shape);
}
