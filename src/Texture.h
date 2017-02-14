//
//  Texture.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Texture_h
#define Texture_h

#include "TextureLoader.h"

class Texture {
public:
    Texture() {};
    Texture(TextureLoader *t) { texture = t; };
    virtual ~Texture() {};
    
    TextureLoader *texture = nullptr;
    
    static void LoadTextures(std::string resourceDir);
    static Texture *hair;
};

#endif /* Texture_h */
