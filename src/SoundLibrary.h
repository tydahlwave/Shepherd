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
    static void playCharge();
    static void playShot();
    static void playRandSheep();
    
private:
 
};

#endif /* SoundLibrary_h */
