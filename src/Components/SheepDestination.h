//
//  SheepDestination.h
//  Shepherd
//
//  Created by Michael Cantrell on 3/11/17.
//
//

#ifndef SheepDestination_h
#define SheepDestination_h

#include "Component.h"

class SheepDestination : public Component {
public:
    SheepDestination() {} ;
    virtual ~SheepDestination() {};
    
     Path *path = nullptr;
    void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) {

    }
};

#endif /* Light_h */
