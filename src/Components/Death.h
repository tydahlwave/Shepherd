//
//  Death.h
//  Shepherd
//
//  Created by Hermes Build Machine on 2/16/17.
//
//

#ifndef Death_h
#define Death_h

class Death : public Component {
public: 
    bool shouldDie = false;
    
    Death() { name = "Death"; };
    virtual ~Death() {};
    
};

#endif /* Death_h */
