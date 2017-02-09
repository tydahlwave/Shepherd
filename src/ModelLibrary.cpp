//
//  ModelLibrary.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/23/17.
//
//

#include "ModelLibrary.h"

Model *ModelLibrary::bunny = nullptr;
Model *ModelLibrary::cube = nullptr;
Model *ModelLibrary::plane = nullptr;
Model *ModelLibrary::sphere = nullptr;
Model *ModelLibrary::sphere_textured = nullptr;
Model *ModelLibrary::boulder1 = nullptr;
Model *ModelLibrary::boulder2 = nullptr;
Model *ModelLibrary::boulder3 = nullptr;
Model *ModelLibrary::wolf = nullptr;

void ModelLibrary::LoadModels(std::string resourceDir) {
    bunny           = new Model(resourceDir + "bunny.obj");
    cube            = new Model(resourceDir + "cube.obj");
    plane           = new Model(resourceDir + "plane.obj");
    sphere          = new Model(resourceDir + "sphere.obj");
    sphere_textured = new Model(resourceDir + "sphere_textured.obj");
    boulder1        = new Model(resourceDir + "Boulder1.obj");
    boulder2        = new Model(resourceDir + "Boulder2.obj");
    boulder3        = new Model(resourceDir + "Boulder3.obj");
    wolf            = new Model(resourceDir + "wolf.obj");
}
