//
//  Time.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Time_h
#define Time_h

#include <chrono>

class Time {
public:
    // Current time in milliseconds
    static long Now() {
        return std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::steady_clock::now().time_since_epoch()).count();
    }
};

#endif /* Time_h */
