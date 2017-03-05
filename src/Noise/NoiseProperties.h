//
//  NoiseProperties.h
//  Shepherd
//
//  Created by Tyler Dahl on 3/4/17.
//
//

#ifndef NoiseProperties_h
#define NoiseProperties_h

#include <time.h>

struct NoiseProperties {
    time_t seed = 0;
    float frequency = 5.0f;
    int octaves = 3;
};

#endif /* NoiseProperties_h */
