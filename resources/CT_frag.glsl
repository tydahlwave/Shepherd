#version 330 core
in vec3 fragPos;
in vec3 fragNor;
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matAmbientColor;
uniform float matShine;
uniform mat4 V;

uniform int useTexture;
uniform sampler2D myTexture;

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

in VS_OUT {
    vec3 fragPos;
    vec3 vertPos;
    vec3 modelNor;
    vec3 vertNor;
    vec3 viewNor;
    vec3 modelPos;
    vec2 vertTex;
} vs_out;


//helper
float stepmix(float edge0, float edge1, float E, float x)
{
    float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);
    return mix(edge0, edge1, T);
}

vec3 ApplyLight(Light light, vec3 vertexN, vec3 viewN, vec3 lightPos) {
    
    // set important material values
    float roughnessValue = 0.8; // 0 : smooth, 1: rough
    float F0 = 0.6; // fresnel reflectance at normal incidence
    float k = 0.3; // fraction of diffuse reflection (specular reflection = 1 - k)
    vec3 lightColor = vec3(0.9, 0.3, 0.9);
    
    
    
    
    vec3 lightN;
    float attenuation = 1.0;
    if(light.position.w == 0.0) {
        //directional light
        lightN = normalize(light.position.xyz);
        attenuation = 1.0; //no attenuation for directional lights
    } else {
        //point light
        lightN = normalize(light.position.xyz - vs_out.modelPos);
        float distanceToLight = length(light.position.xyz - vs_out.modelPos);
        
        //attenuation = 1.0 / (1.0 + light.attenuation * pow(distanceToLight, 2.0));
        attenuation = clamp( 10.0 / (1.0 + light.attenuation * distanceToLight), 0.0, 1.0);
        
        //cone restrictions (affects attenuation)
        float lightToSurfaceAngle = degrees(acos(dot(-lightN, normalize(light.coneDirection))));
        if(lightToSurfaceAngle > light.coneAngle){
            attenuation = 0.0;
        }
    }
    
    const float A = 0.1;
    const float B = 0.3;
    const float C = 0.6;
    const float D = 1.0;
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
    //Else regular bands
    else if (df < A)
        df = 0.0;
    else if (df < B)
        df = B;
    else if (df < C)
        df = C;
    else
        df = D;
    
    float sf = max(0.0, dot(vertexN, normalize(lightN + vec3(0,0,1))));
    
    sf = pow(sf, matShine);
    
    sf = step(0.5, sf);
    
    vec3 ambientColor = matAmbientColor;
    vec3 diffuseColor = matDiffuseColor;
    vec3 specularColor = matSpecularColor;
    if (useTexture == 0) {
        ambientColor = diffuseColor = specularColor = texture(myTexture, vs_out.vertTex).xyz / 2;
    }
    
    //COOK TORRANCE STUFF
    // do the lighting calculation for each fragment.
    float NdotL = max(dot(vertexN, lightN), 0.0);
    
    float specC = 0.0;
    if(NdotL > 0.0)
    {
        vec3 eyeDir = normalize(viewN);
        
        // calculate intermediary values
        vec3 halfVector = normalize(lightN + eyeDir);
        float NdotH = max(dot(vertexN, halfVector), 0.0);
        float NdotV = max(dot(vertexN, eyeDir), 0.0); // note: this could also be NdotL, which is the same value
        float VdotH = max(dot(eyeDir, halfVector), 0.0);
        float mSquared = roughnessValue * roughnessValue;
        
        // geometric attenuation
        float NH2 = 2.0 * NdotH;
        float g1 = (NH2 * NdotV) / VdotH;
        float g2 = (NH2 * NdotL) / VdotH;
        float geoAtt = min(1.0, min(g1, g2));
        
        // roughness (or: microfacet distribution function)
        // beckmann distribution function
        float r1 = 1.0 / ( 4.0 * mSquared * pow(NdotH, 4.0));
        float r2 = (NdotH * NdotH - 1.0) / (mSquared * NdotH * NdotH);
        float roughness = r1 * exp(r2);
        
        // fresnel
        // Schlick approximation
        float fresnel = pow(1.0 - VdotH, 5.0);
        fresnel *= (1.0 - F0);
        fresnel += F0;
        
        specC = (fresnel * geoAtt * roughness) / (NdotV * NdotL * 3.14);
    }
    
    
    //ambient
    vec3 ambient = light.ambientCoefficient * ambientColor * light.intensities;
    
    //diffuse
    vec3 diffuse = diffuseColor * max(dot(vertexN, lightN), 0) * light.intensities;
    //vec3 diffuse = diffuseColor * df;
    
    //specular
    vec3 specular = specularColor  * NdotL * (k + specC * (1.0 - k)) * sf;
    
    //linear color (color before gamma correction)
    if (light.position.w == 0)
        return ambient + (diffuse + specular);
        //return specular;
    else
        return ambient + attenuation*(diffuse + specular);
        //return attenuation * specular;
}


void main()
{
    // Normalize the vectors
    vec3 vertexN = normalize(vs_out.modelNor);
    vec3 viewN = normalize(vs_out.viewNor);
    //combine color from all the lights
    vec3 linearColor = matAmbientColor;//vec3(0);
    for(int i = 0; i < numLights; ++i){
        vec3 pos = vec3(V * vec4(vec3(allLights[i].position), 1));
        linearColor += ApplyLight(allLights[i], vertexN, viewN, pos);
    }
    
    float edgeDetection = (dot(viewN, vs_out.modelNor) > 0.3) ? 1 : 0;
    
    color = vec4(linearColor, 1.0);
    //final color (after gamma correction)
    //vec3 gamma = vec3(1.0/2.2);
    //color = vec4(pow(totalPhong, gamma), 1.0);
}
