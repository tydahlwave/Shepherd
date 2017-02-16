#version 330 core
in vec3 fragPos;
in vec3 fragNor;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 sunDir;
uniform vec3 sunColor;
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matAmbientColor;
uniform float matShine;
uniform vec3 cameraPos;
uniform mat4 M;

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
in vec3 lightNormal;
in vec3 viewNormal;
in vec3 sunVertexNormal;

vec3 ApplyLight(Light light, vec3 surfaceColor, vec3 normal, vec3 surfacePos, vec3 surfaceToCamera) {
    vec3 surfaceToLight;
    float attenuation = 1.0;
    if(light.position.w == 0.0) {
        //directional light
        surfaceToLight = normalize(light.position.xyz);
        attenuation = 1.0; //no attenuation for directional lights
    } else {
        //point light
        surfaceToLight = normalize(light.position.xyz - surfacePos);
        float distanceToLight = length(light.position.xyz - surfacePos);
        attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2));
        
        //cone restrictions (affects attenuation)
        float lightToSurfaceAngle = degrees(acos(dot(-surfaceToLight, normalize(light.coneDirection))));
        if(lightToSurfaceAngle > light.coneAngle){
            attenuation = 0.0;
        }
    }
    
    //ambient
    vec3 ambient = light.ambientCoefficient * surfaceColor.rgb * light.intensities;
    
    //diffuse
    float diffuseCoefficient = max(0.0, dot(normal, surfaceToLight));
    vec3 diffuse = diffuseCoefficient * surfaceColor.rgb * light.intensities;
    
    //specular
    float specularCoefficient = 0.0;
    if(diffuseCoefficient > 0.0)
        specularCoefficient = pow(max(0.0, dot(surfaceToCamera, reflect(-surfaceToLight, normal))), matShine);
    vec3 specular = specularCoefficient * matSpecularColor * light.intensities;
    
    //linear color (color before gamma correction)
    return ambient + attenuation*(diffuse + specular);
}


void main()
{
    // Normalize the vectors
    vec3 vertexN = normalize(vertexNormal);
    vec3 viewN = normalize(viewNormal);
    vec3 surfacePos = vec3(M * vec4(fragPos, 1));
    vec3 surfaceToCamera = normalize(cameraPos - fragPos);
    
    vec3 lightN = normalize(lightNormal);
    vec3 sunVertexN = normalize(sunVertexNormal);
    
    // Calculate diffuse color
    vec3 diffuse = matDiffuseColor * max(dot(vertexN, lightN), 0) * lightColor;
    
    // Calculate specular color
    float alpha = matShine;
    vec3 halfValue = normalize(viewN + lightN);
    vec3 specular = matSpecularColor * pow(max(dot(vertexN, halfValue), 0), alpha) * lightColor;
    
    // Calculate ambient color
    vec3 ambient = matAmbientColor;
    
    // Calculate distance attenuation
    float a = 0;
    float b = 0.5;
    float c = 0;
    float distanceToLight = length(lightPos - fragPos);
    float distanceAttenuation = 1 / (a + b * distanceToLight + c * pow(distanceToLight, 2));
    
    // Calculate phong from lights
    vec3 lightsPhong = distanceAttenuation * (diffuse + specular);
    
    // Calculate phong from sun
    vec3 sunDir = normalize(sunDir);
    vec3 sunDiffuse = matDiffuseColor * max(dot(sunVertexN, sunDir), 0) * sunColor;
    halfValue = normalize(viewN + sunDir);
    vec3 sunSpecular = matSpecularColor * pow(max(dot(sunVertexN, halfValue), 0), alpha) * sunColor;
    // No distance attenuation, but the vector magnitude affects the brightness
    vec3 sunPhong = length(sunDir) * (sunDiffuse + sunSpecular);
    
    // This provides a cool effect if I use it instead of the phong calculation
    // vec3 sunLight = max(dot(sunVertexN, normalize(sunDir)), 0) * sunColor;
    
    //combine color from all the lights
    vec3 linearColor = vec3(0);
    for(int i = 0; i < numLights; ++i){
        linearColor += ApplyLight(allLights[i], matAmbientColor.rgb, vertexN, fragPos, surfaceToCamera);
    }
    
    vec3 totalPhong = ambient + lightsPhong + sunPhong;
    
    color = vec4(totalPhong, 1.0);
    //final color (after gamma correction)
    //vec3 gamma = vec3(1.0/2.2);
    //color = vec4(pow(totalPhong, gamma), 1.0);
}
