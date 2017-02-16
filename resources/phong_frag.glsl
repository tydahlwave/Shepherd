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

vec3 ApplyLight(Light light, vec3 vertexN, vec3 viewN, vec3 lightPos) {
    vec3 lightN;
    float attenuation = 1.0;
    if(light.position.w == 0.0) {
        //directional light
        lightN = normalize(lightPos);
        attenuation = 1.0; //no attenuation for directional lights
    } else {
        //point light
        lightN = normalize(lightPos - fragPos);
        float a = 0;
        float b = 0.5;
        float c = 0;
        float distanceToLight = length(lightPos - fragPos);
        float attenuation = 1 / (a + b * distanceToLight + c * pow(distanceToLight, 2));
        
        //cone restrictions (affects attenuation)
        float lightToSurfaceAngle = degrees(acos(dot(-lightN, normalize(light.coneDirection))));
        if(lightToSurfaceAngle > light.coneAngle){
            attenuation = 0.0;
        }
    }
    
    //ambient
    vec3 ambient = light.ambientCoefficient * matAmbientColor * light.intensities;
    
    //diffuse
    float diffuseCoefficient = max(0.0, dot(vertexN, lightN));
    vec3 diffuse = matDiffuseColor * diffuseCoefficient * light.intensities;
    
    //specular
    float alpha = matShine;
    vec3 halfValue = normalize(viewN + lightN);
    
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(vertexN, halfValue)), alpha);
    vec3 specular = specularCoefficient * matSpecularColor * light.intensities;
    
    //linear color (color before gamma correction)
    return ambient + attenuation*(diffuse + specular);
}


void main()
{
    // Normalize the vectors
    vec3 vertexN = normalize(vertexNormal);
    vec3 viewN = normalize(viewNormal);
        //combine color from all the lights
    vec3 linearColor = vec3(0);
    for(int i = 0; i < numLights; ++i){
        vec3 pos = vec3(V * vec4(vec3(allLights[i].position), 1));
        linearColor += ApplyLight(allLights[i], vertexN, viewN, pos);
    }
    
    //vec3 totalPhong = ambient + lightsPhong + sunPhong;
    
    color = vec4(linearColor, 1.0);
    //final color (after gamma correction)
    //vec3 gamma = vec3(1.0/2.2);
    //color = vec4(pow(totalPhong, gamma), 1.0);
}
