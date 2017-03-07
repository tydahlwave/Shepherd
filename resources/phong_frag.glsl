#version 330 core
in vec3 fragPos;
in vec3 fragNor;
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matAmbientColor;
uniform float matShine;
uniform mat4 V;

#define MAX_LIGHTS 10
uniform int numLights;
uniform struct Light {
    vec4 position;
    vec3 intensities;
    float attenuation;
    float ambientCoefficient;
    float coneAngle;
    vec3 coneDirection;
} allLights[MAX_LIGHTS];

out vec4 color;

in vec3 vertexNormal;
in vec3 viewNormal;

in vec3 modelPos;
in vec3 modelNor;

vec3 ApplyLight(Light light, vec3 vertexN, vec3 viewN, vec3 lightPos) {
    
    vec3 lightN;
    float attenuation = 1.0;
    if(light.position.w == 0.0) {
        //directional light
        lightN = normalize(lightPos);
        attenuation = 1.0; //no attenuation for directional lights
    } else {
        //point light
        lightN = normalize(light.position.xyz - modelPos);
        float distanceToLight = length(light.position.xyz - modelPos);
        
        //attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2.0));
        attenuation = clamp( 10.0 / (1.0 + light.attenuation * distanceToLight), 0.0, 1.0);
        
        //cone restrictions (affects attenuation)
        float lightToSurfaceAngle = degrees(acos(dot(-lightN, normalize(light.coneDirection))));
        if(lightToSurfaceAngle > light.coneAngle){
            attenuation = 0.0;
        }
    }
    
    //ambient
    vec3 ambient = light.ambientCoefficient * matAmbientColor * light.intensities;
    
    //diffuse
    vec3 diffuse = matDiffuseColor * max(dot(vertexN, lightN), 0) * light.intensities;
    
    //specular
    float alpha = matShine;
    vec3 halfValue = normalize(viewN + lightN);
    vec3 specular = matSpecularColor * pow(max(dot(vertexN, halfValue), 0), alpha) * light.intensities;
    
    //linear color (color before gamma correction)
    return ambient + attenuation*(diffuse + specular);
}


void main()
{
    // Normalize the vectors
    vec3 vertexN = normalize(modelNor);
    vec3 viewN = normalize(viewNormal);
    //combine color from all the lights
    vec3 linearColor = vec3(0);
    for(int i = 0; i < numLights; ++i){
        vec3 pos = vec3(V * vec4(vec3(allLights[i].position), 1));
        linearColor += ApplyLight(allLights[i], vertexN, viewN, pos);
    }
    
    
    color = vec4(linearColor, 1.0);
}
