//
//  TextureLibrary.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 1/29/17.
//
//

#include "TextureLibrary.h"

Texture *TextureLibrary::crate = nullptr;
Texture *TextureLibrary::grass = nullptr;
Texture *TextureLibrary::mountain = nullptr;
Texture *TextureLibrary::snow = nullptr;

void TextureLibrary::LoadTextures(std::string resourceDir) {
    crate = new Texture("Crate", resourceDir + "crate.bmp");
    crate->Load();
    
    grass = new Texture("Grass", resourceDir + "grass_seamless.jpg");
    grass->Load();
    
    mountain = new Texture("Mountain", resourceDir + "rock_seamless.jpg");
    mountain->Load();
    
    snow = new Texture("Snow", resourceDir + "snow_seamless.jpg");
    snow->Load();
}
