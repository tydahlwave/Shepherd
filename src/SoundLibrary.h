//
//  SoundLibrary.hpp
//  Shepherd
//
//  Created by Hermes Build Machine on 2/15/17.
//
//

#ifndef SoundLibrary_h
#define SoundLibrary_h

#include <stdio.h>
#include "AudioEngine.h"

class SoundLibrary
{
public:
    SoundLibrary();
    ~SoundLibrary();
    
    static int chargeChannel;
    static int walkChannel;
    static void playWalk();
    static void stopWalk();
    static void playCharge();
    static void playShot();
    static void playRandSheep();
    static void playRandFun();
    static void playWolfHurt();
    static void playWolfSpawn();
    static void playTreeHit();
    static void playRockHit();
    static void playPing();
    static void playPong();
    
private:
 
};

#endif /* SoundLibrary_h */
