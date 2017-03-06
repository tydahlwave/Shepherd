//
//  Light.h
//  Shepherd
//
//  Created by Michael Cantrell on 3/3/17.
//
//

#ifndef Light_h
#define Light_h

#include "Component.h"

/*
 Represents a point light
 w == 0 in the position indications a directional light
 */
struct LightStruct {
    glm::vec4 position;
    glm::vec3 intensities; //a.k.a. the color of the light
    float attenuation;
    float ambientCoefficient;
    float coneAngle;
    glm::vec3 coneDirection;
};

class Light : public Component {
public:
    Light();
    Light(bool isDirectional, glm::vec3 intensities, float attenuation,float ambientCoefficient,float coneAngle, glm::vec3 coneDirection);
    virtual ~Light() {};
    
    bool isDirectional;
    glm::vec3 intensities; //a.k.a. the color of the light
    float attenuation;
    float ambientCoefficient;
    float coneAngle;
    glm::vec3 coneDirection;
    LightStruct GetLight();
    void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) {
        writer.Key("Light");
        writer.StartObject();
        writer.Key("IsDirectional");
        writer.Bool(isDirectional);
        writer.Key("Intensities");
        writer.StartArray();
        writer.Double(intensities.x);
        writer.Double(intensities.y);
        writer.Double(intensities.z);
        writer.EndArray();
        writer.Key("Attenuation");
        writer.Double(attenuation);
        writer.Key("AmbientCoefficient");
        writer.Double(ambientCoefficient);
        writer.Key("ConeAngle");
        writer.Double(coneAngle);
        writer.Key("ConeDirection");
        writer.StartArray();
        writer.Double(coneDirection.x);
        writer.Double(coneDirection.y);
        writer.Double(coneDirection.z);
        writer.EndArray();
        writer.EndObject();
    }
};

#endif /* Light_h */
