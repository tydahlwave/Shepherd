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
Texture *TextureLibrary::sheepIcon = nullptr;
Texture *TextureLibrary::wolfIcon = nullptr;
Texture *TextureLibrary::goalIcon = nullptr;

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
    
    hair = new Texture("Hair");
    hair->Load(resourceDir + "hair.bmp");
    
    sheepIcon = new Texture("SheepIcon");
    sheepIcon->Load(resourceDir + "sheep_icon.png");
    
    wolfIcon = new Texture("WolfIcon");
    wolfIcon->Load(resourceDir + "wolf_icon.png");
    
    goalIcon = new Texture("GoalIcon");
    goalIcon->Load(resourceDir + "goal_icon.png");
}
