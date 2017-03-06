//
//  TextureLibrary.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef TextureLibrary_h
#define TextureLibrary_h

#include "Texture.h"

class TextureLibrary {
public:
    static void LoadTextures(std::string resourceDir);
    static Texture *texTest;
};

#endif /* Texture_h */
