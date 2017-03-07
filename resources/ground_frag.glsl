#version 330 core
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

in vec3 fragPos;
in vec3 fragNor;
in vec3 viewNor;

in vec3 modelPos;
in vec3 modelNor;

out vec4 color;

//helper
float stepmix(float edge0, float edge1, float E, float x)
{
    float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);
    return mix(edge0, edge1, T);
}

vec3 ApplyLight(Light light, vec3 vertexN, vec3 viewN, vec3 lightPos) {
    
    vec3 lightN;
    float attenuation = 1.0;
    if(light.position.w == 0.0) {
        //directional light
        lightN = normalize(light.position.xyz);
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
    
    const float A = 0.1;
    const float B = 0.2;
    const float C = 0.4;
    const float D = 0.6;
    const float E = 0.8;
    const float F = 1.0;
    
    float df = max(0.0, dot(vertexN, lightN));
    float W = fwidth(df);
    
    //Stepmix!!!!
    if (df > A - W && df < A + W)
    stepmix(A, B, W, df);
    
    else if (df > B - W && df < B + W)
    stepmix(B, C, W, df);
    
    else if (df > C - W && df < C + W)
    stepmix(C, D, W, df);
    
    else if (df > D - W && df < D + W)
    stepmix(D, E, W, df);
    
    else if (df > E - W && df < E + W)
    stepmix(E, F, W, df);
    //Else regular bands
    else if (df < A)
    df = 0.0;
    else if (df < B)
    df = B;
    else if (df < C)
    df = C;
    else if (df < D)
    df = D;
    else if (df < E)
    df = E;
    else
    df = F;
    
    float sf = max(0.0, dot(vertexN, normalize(lightN + vec3(0,0,1))));
    
    sf = pow(sf, matShine);
    
    sf = step(0.5, sf);
    
    
    //ambient
    vec3 ambient = light.ambientCoefficient * matAmbientColor * light.intensities;
    
    //diffuse
    vec3 diffuse = df * matDiffuseColor * max(dot(vertexN, lightN), 0) * light.intensities;
    //vec3 diffuse = matDiffuseColor * df;
    
    //specular
    float alpha = matShine;
    vec3 halfValue = normalize(viewN + lightN);
    vec3 specular = vec3(0);//sf * matSpecularColor * pow(max(dot(vertexN, halfValue), 0), alpha) * light.intensities;
    //vec3 specular = matSpecularColor  * sf;
    
    //linear color (color before gamma correction)
    return ambient + attenuation*(diffuse + specular);
}


void main() {
    // Normalize the vectors
    vec3 vertexN = normalize(modelNor);
    vec3 viewN = normalize(viewNor);
    //combine color from all the lights
    vec3 linearColor = vec3(0);
    for(int i = 0; i < numLights; ++i){
        vec3 pos = vec3(V * vec4(vec3(allLights[i].position), 1));
        linearColor += ApplyLight(allLights[i], vertexN, viewN, pos);
    }
    
    float edgeDetection = (dot(viewN, vertexN) > 0.3) ? 1 : 0;
    color = vec4(linearColor, 1.0);
}
