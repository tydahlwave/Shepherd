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
Model *ModelLibrary::button = nullptr;
Model *ModelLibrary::sphere = nullptr;
Model *ModelLibrary::sphere_textured = nullptr;
Model *ModelLibrary::boulder1 = nullptr;
Model *ModelLibrary::boulder2 = nullptr;
Model *ModelLibrary::boulder3 = nullptr;
Model *ModelLibrary::wolf = nullptr;
Model *ModelLibrary::sheep = nullptr;
Model *ModelLibrary::title = nullptr;
Model *ModelLibrary::tree1 = nullptr;
Model *ModelLibrary::tree2 = nullptr;
Model *ModelLibrary::tree3 = nullptr;
Model *ModelLibrary::tree4 = nullptr;
Model *ModelLibrary::tree5 = nullptr;
Model *ModelLibrary::tree6 = nullptr;
Model *ModelLibrary::player = nullptr;
Model *ModelLibrary::gadget = nullptr;
Model *ModelLibrary::menuSign = nullptr;
Model *ModelLibrary::lantern = nullptr;
Model *ModelLibrary::playSign = nullptr;
Model *ModelLibrary::exitSign = nullptr;

void ModelLibrary::LoadModels(std::string resourceDir) {
    bunny           = new Model(resourceDir + "bunny.obj");
    cube            = new Model(resourceDir + "cube.obj");
    plane           = new Model(resourceDir + "plane.obj");
	button			= new Model(resourceDir + "button.obj");
    sphere          = new Model(resourceDir + "sphere.obj");
    sphere_textured = new Model(resourceDir + "sphere_textured.obj");
    boulder1        = new Model(resourceDir + "Boulder1.obj");
    boulder2        = new Model(resourceDir + "Boulder2.obj");
    boulder3        = new Model(resourceDir + "Boulder3.obj");
    wolf            = new Model(resourceDir + "wolf.obj");
    title           = new Model(resourceDir + "title.obj");
    tree1           = new Model(resourceDir + "tree1.obj");
    tree2           = new Model(resourceDir + "tree2.obj");
    tree3           = new Model(resourceDir + "tree3.obj");
    tree4           = new Model(resourceDir + "tree4.obj");
    tree5           = new Model(resourceDir + "tree5.obj");
    tree6           = new Model(resourceDir + "tree6.obj");
    player          = new Model(resourceDir + "player.obj");
    gadget          = new Model(resourceDir + "gadget.obj");
    sheep           = new Model(resourceDir + "sheep.obj");
	menuSign		= new Model(resourceDir + "title.obj");
    lantern         = new Model(resourceDir + "lantern.obj");
    playSign        = new Model(resourceDir + "Play.obj");
    exitSign        = new Model(resourceDir + "Exit.obj");
}
