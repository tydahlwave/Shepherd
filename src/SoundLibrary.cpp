//
//  SoundLibrary.cpp
//  Shepherd
//
//  Created by Hermes Build Machine on 2/15/17.
//
//Sound Lib class to abstract functionality of the AudioEngine

#include "SoundLibrary.h"

int SoundLibrary::chargeChannel = 0;
int SoundLibrary::walkChannel = 0;

void SoundLibrary::playWalk(){
    if(walkChannel <= 0)
    {
        walkChannel = CAudioEngine::instance()->PlaySound("grassRun.wav");
        CAudioEngine::instance()->SetChannelvolume(walkChannel, 10);
    }
}

void SoundLibrary::stopWalk()
{
    CAudioEngine::instance()->toggleSound(walkChannel, true);
    walkChannel = -1;
}

//Charge shot
void SoundLibrary::playCharge(){
    chargeChannel = CAudioEngine::instance()->PlaySound("growl.wav");
}

//end charge, shoot
void SoundLibrary::playShot(){
    CAudioEngine::instance()->toggleSound(chargeChannel, true);
    int v2 = rand() % 3 + 1;
    switch(v2){
        case 1:{
            CAudioEngine::instance()->PlaySound("roar.wav");
        }
        case 2:{
            CAudioEngine::instance()->PlaySound("roar2.wav");
        }
        case 3:{
            CAudioEngine::instance()->PlaySound("roar3.wav");
        }
    }
}

void SoundLibrary::playRandSheep(){
    int v2 = rand() % 4 + 1;
    int sheep;
    switch(v2){
        case 2:{
            sheep = CAudioEngine::instance()->PlaySound("sheep2.wav");
        }
        case 3:{
            sheep = CAudioEngine::instance()->PlaySound("sheep3.wav");
        }
        case 4:{
            sheep = CAudioEngine::instance()->PlaySound("sheep4.wav");
        }
        case 5:{
            sheep = CAudioEngine::instance()->PlaySound("sheep5.wav");
        }
        default:{
            sheep = CAudioEngine::instance()->PlaySound("sheep.wav");
        }
    }
    CAudioEngine::instance()->SetChannelvolume(sheep, .2);
}

void SoundLibrary::playRandFun(){
    int v2 = rand() % 4 + 1;
    int sheep;
    switch(v2){
        case 2:{
            sheep = CAudioEngine::instance()->PlaySound("fun1.wav");
        }
        case 3:{
            sheep = CAudioEngine::instance()->PlaySound("fun2.wav");
        }
        case 4:{
            sheep = CAudioEngine::instance()->PlaySound("fun3.wav");
        }
        case 5:{
            sheep = CAudioEngine::instance()->PlaySound("fun4.wav");
        }
    }
    CAudioEngine::instance()->SetChannelvolume(sheep, .2);
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

void SoundLibrary::playPing(){
    CAudioEngine::instance()->PlaySound("ping.wav");
}

void SoundLibrary::playPong(){
    CAudioEngine::instance()->PlaySound("pong.wav");
}

void SoundLibrary::playYay(){
    CAudioEngine::instance()->PlaySound("yay.wav");
}
