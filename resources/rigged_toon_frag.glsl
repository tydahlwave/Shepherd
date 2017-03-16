#version 330 core
in vec3 fragPos;
in vec3 fragNor;
in vec4 we;
in vec4 id;

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

//interpolation helper
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
    
    const float A = 0.05;
    const float B = 0.1;
    const float C = 0.3;
    const float D = 0.5;
    const float E = 0.8;
    const float F = 1.0;
    float distanceToLight = length(lightPos - fragPos);
    float df = max(0.0, dot(vertexN, lightN));
    float W = fwidth(df);
    
    //Stepmix!!!! for anti aliasing
    if (df > A - W && df < A + W)
        df = stepmix(A, B, W, df);
    
    else if (df > B - W && df < B + W)
        df = stepmix(B, C, W, df);
    
    else if (df > C - W && df < C + W)
        df = stepmix(C, D, W, df);
    
    else if (df > D - W && df < D + W)
        df = stepmix(D, E, W, df);
    
    else if (df > E - W && df < E + W)
        df = stepmix(E, F, W, df);
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
    //df = df * (20 / (.5 * distanceToLight));
    
    float sf = max(0.0, dot(vertexN, normalize(lightN + vec3(0,0,1))));
    
    sf = pow(sf, matShine * 2);
    
    sf = step(0.2, sf);
    
    //sf = sf *  (1 / (.5 * distanceToLight));
    
    //ambient
    vec3 ambient = light.ambientCoefficient * matAmbientColor * light.intensities;
    
    //diffuse
    vec3 diffuse = matDiffuseColor * max(dot(vertexN, lightN), 0) * light.intensities * df;
    //vec3 diffuse = matDiffuseColor * df;
    
    //specular
    float alpha = matShine;
    vec3 halfValue = normalize(viewN + lightN);
    vec3 specular = matSpecularColor * pow(max(dot(vertexN, halfValue), 0), alpha) * light.intensities * sf;
    //vec3 specular = matSpecularColor  * sf;
    
    //linear color (color before gamma correction)
    return ambient + (diffuse + specular);
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
        //vec3 toAdd = ApplyLight(allLights[i], vertexN, viewN, pos);
        
        //linearColor += toAdd;
    }
    
    float edgeDetection = (dot(viewN, vertexN) > 0.3) ? 1 : 0;
    vec4 weightsColor = vec4(we.xyz,1.0);
    
    color = weightsColor;
    
    //color = vec4(edgeDetection*linearColor, 1.0);
    //final color (after gamma correction)
    //vec3 gamma = vec3(1.0/2.2);
    //color = vec4(pow(totalPhong, gamma), 1.0);
}