//
//  SoundLibrary.cpp
//  Shepherd
//
//  Created by Hermes Build Machine on 2/15/17.
//
//Sound Lib class to abstract functionality of the AudioEngine

#include "SoundLibrary.h"

int SoundLibrary::chargeChannel = 0;



//Charge shot
void SoundLibrary::playCharge(){
    chargeChannel = CAudioEngine::instance()->PlaySound("charge.wav");
}
//end charge, shoot
void SoundLibrary::playShot(){
    CAudioEngine::instance()->toggleSound(chargeChannel, true);
    CAudioEngine::instance()->PlaySound("shot.wav");
}

void SoundLibrary::playRandSheep(){
    int v2 = rand() % 5 + 1;
    switch(v2){
        case 2:{
            CAudioEngine::instance()->PlaySound("sheep2.wav");
        }
        case 3:{
            CAudioEngine::instance()->PlaySound("sheep3.wav");
        }
        case 4:{
            CAudioEngine::instance()->PlaySound("sheep4.wav");
        }
        case 5:{
            CAudioEngine::instance()->PlaySound("sheep5.wav");
        }
        default:{
            CAudioEngine::instance()->PlaySound("sheep.wav");
        }
    }
}

void SoundLibrary::playWolfHurt(){
    CAudioEngine::instance()->PlaySound("wolfOw.wav");
    
}
void SoundLibrary::playWolfSpawn(){
    CAudioEngine::instance()->PlaySound("wolfGrowl.wav");
}
 void SoundLibrary::playTreeHit(){
    CAudioEngine::instance()->PlaySound("treeMove.wav");
}
 void SoundLibrary::playRockHit(){
    CAudioEngine::instance()->PlaySound("rock.wav");
}
