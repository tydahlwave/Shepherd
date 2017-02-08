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
Mesh *Mesh::sphere = nullptr;
Mesh *Mesh::sphere_textured = nullptr;
Mesh *Mesh::boulder1 = nullptr;
Mesh *Mesh::boulder2 = nullptr;
Mesh *Mesh::boulder3 = nullptr;
Mesh *Mesh::wolf = nullptr;

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
    
    shape = new Shape();
    shape->loadMesh(resourceDir + "sphere.obj");
    shape->resize();
    shape->init();
    sphere = new Mesh(shape);

    shape = new Shape();
    shape->loadMesh(resourceDir + "sphere_textured.obj");
    shape->resize();
    shape->init();
    sphere_textured = new Mesh(shape);
    
    shape = new Shape();
    shape->loadMesh(resourceDir + "Boulder1.obj");
    shape->resize();
    shape->init();
    boulder1 = new Mesh(shape);
    shape = new Shape();
    shape->loadMesh(resourceDir + "Boulder2.obj");
    shape->resize();
    shape->init();
    boulder2 = new Mesh(shape);
    shape = new Shape();
    shape->loadMesh(resourceDir + "Boulder3.obj");
    shape->resize();
    shape->init();
    boulder3 = new Mesh(shape);
    
    shape = new Shape();
    shape->loadMesh(resourceDir + "wolf.obj");
    shape->resize();
    shape->init();
    wolf = new Mesh(shape);
}
