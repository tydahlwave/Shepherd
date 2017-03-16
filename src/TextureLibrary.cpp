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
Texture *TextureLibrary::normalMap = nullptr;
Texture *TextureLibrary::dudvMap = nullptr;

void TextureLibrary::LoadTextures(std::string resourceDir) {
    crate = new Texture("Crate", resourceDir + "crate.bmp");
    crate->Load();
    
    grass = new Texture("Grass", resourceDir + "grass_seamless2.jpg");
    grass->Load();
    
    mountain = new Texture("Mountain", resourceDir + "rock_seamless.jpg");
    mountain->Load();
    
    snow = new Texture("Snow", resourceDir + "snow_seamless.jpg");
    snow->Load();

	startButton = new Texture("StartButton", resourceDir + "buttons/button_start.jpg");
	startButton->Load();


	quitButton = new Texture("QuitButton", resourceDir + "buttons/button_quit.jpg");
	quitButton->Load();
    
    hair = new Texture("Hair", resourceDir + "hair.bmp");
    hair->Load();

	circle = new Texture("Circle", resourceDir + "circle.jpg");
	circle->Load();

	snowflake = new Texture("Snowflake", resourceDir + "snowflake.png");
	snowflake->Load();

	blood = new Texture("Blood", resourceDir + "blood.png");
	blood->Load();

	normalMap = new Texture("NormalMap", resourceDir + "normalMap.bmp");
	normalMap->Load();

	dudvMap = new Texture("DudvMap", resourceDir + "waterdudv.bmp");
	dudvMap->Load();
}
