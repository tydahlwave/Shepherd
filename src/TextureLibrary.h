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
    static Texture *crate;
    static Texture *grass;
    static Texture *mountain;
    static Texture *snow;
	static Texture *startButton;
	static Texture *quitButton;
    static Texture *hair;

};

#endif /* Texture_h */
