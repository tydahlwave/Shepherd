//
//  Light.cpp
//  Shepherd
//
//  Created by Michael Cantrell on 3/3/17.
//
//

#include "Components/Light.h"
#include "GameObject.h"

Light::Light() : Light(false, glm::vec3(1), 0.1f,0.15f, 360, glm::vec3(0,1,0)) {
}

Light::Light(bool isDirectional, glm::vec3 intensities, float attenuation,float ambientCoefficient,float coneAngle, glm::vec3 coneDirection) {
    this->isDirectional = isDirectional;
    this->intensities = intensities;
    this->attenuation = attenuation;
    this->ambientCoefficient = ambientCoefficient;
    this->coneAngle = coneAngle;
    this->coneDirection = coneDirection;
}

LightStruct Light::GetLight() {
    float w = 1.0;
    if(isDirectional) w = 0.0;
    LightStruct light;
    light.position = vec4(gameObject->transform->GetPosition(), w);
    light.intensities = intensities; //a.k.a. the color of the light
    light.attenuation = attenuation;
    light.ambientCoefficient = ambientCoefficient;
    light.coneAngle = coneAngle;
    light.coneDirection = coneDirection;
    return light;
}


