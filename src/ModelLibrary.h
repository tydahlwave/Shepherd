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
    static Model *sphere;
    static Model *sphere_textured;
    static Model *boulder1;
    static Model *boulder2;
    static Model *boulder3;
    static Model *wolf;
};

#endif /* ModelLibrary_h */
