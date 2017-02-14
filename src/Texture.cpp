//
//  Texture.cpp
//  Shepherd
//
//  Created by Tyler Dahl on 1/29/17.
//
//

#include "Texture.h"

Texture *Texture::hair = nullptr;

void Texture::LoadTextures(std::string resourceDir) {
    TextureLoader *texture = new TextureLoader();
    texture->setFilename(resourceDir + "crate.bmp");
    texture->setUnit(0);
    texture->setName("Texture");
    texture->init();
    hair = new Texture(texture);
}
