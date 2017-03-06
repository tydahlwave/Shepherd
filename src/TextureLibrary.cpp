//
//  TextureLibrary.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 1/29/17.
//
//

#include "TextureLibrary.h"

Texture *TextureLibrary::texTest = nullptr;

void TextureLibrary::LoadTextures(std::string resourceDir) {
    texTest = new Texture(resourceDir + "crate.bmp");
    texTest->Load();
}
