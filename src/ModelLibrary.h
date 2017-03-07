//
//  ModelLibrary.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef ModelLibrary_h
#define ModelLibrary_h

#include <string>

#include "Model.h"

class ModelLibrary {
public:
    static void LoadModels(std::string resourceDir);
    static Model *bunny;
    static Model *cube;
    static Model *plane;
	static Model *button;
    static Model *sphere;
    static Model *sphere_textured;
    static Model *boulder1;
    static Model *boulder2;
    static Model *boulder3;
    static Model *wolf;
    static Model *title;
    static Model *tree1;
    static Model *tree2;
    static Model *tree3;
    static Model *tree4;
    static Model *tree5;
    static Model *tree6;
    static Model *player;
    static Model *gadget;
    static Model *sheep;
	static Model *menuSign;
    static Model *lantern;
    static Model *playSign;
    static Model *exitSign;
};

#endif /* ModelLibrary_h */
