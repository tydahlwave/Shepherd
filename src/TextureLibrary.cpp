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
Texture *TextureLibrary::startButton = nullptr;
Texture *TextureLibrary::quitButton = nullptr;
Texture *TextureLibrary::hair = nullptr;
Texture *TextureLibrary::circle = nullptr;
Texture *TextureLibrary::snowflake = nullptr;
Texture *TextureLibrary::blood = nullptr;
Texture *TextureLibrary::dust = nullptr;
Texture *TextureLibrary::normalMap = nullptr;
Texture *TextureLibrary::dudvMap = nullptr;

void TextureLibrary::LoadTextures(std::string resourceDir) {
    crate = new Texture("Crate");
    crate->Load(resourceDir + "crate.bmp");
    
    grass = new Texture("Grass");
    grass->Load(resourceDir + "grass_seamless.jpg");
    
    mountain = new Texture("Mountain");
    mountain->Load(resourceDir + "rock_seamless.jpg");
    
    snow = new Texture("Snow");
    snow->Load(resourceDir + "snow_seamless.jpg");

	startButton = new Texture("StartButton");
	startButton->Load(resourceDir + "buttons/button_start.jpg");

	quitButton = new Texture("QuitButton");
	quitButton->Load(resourceDir + "buttons/button_quit.jpg");
    
	circle = new Texture("Circle");
	circle->Load(resourceDir + "circle.jpg");

	snowflake = new Texture("Snowflake");
	snowflake->Load(resourceDir + "snowflake.png");

	blood = new Texture("Blood");
	blood->Load(resourceDir + "blood.png");

	dust = new Texture("Dust");
	dust->Load(resourceDir + "dust.png");

	normalMap = new Texture("NormalMap");
	normalMap->Load(resourceDir + "normalMap.bmp");

	dudvMap = new Texture("DudvMap");
	dudvMap->Load(resourceDir + "waterdudv.bmp");

    hair = new Texture("Hair");
    hair->Load(resourceDir + "hair.bmp");
}
