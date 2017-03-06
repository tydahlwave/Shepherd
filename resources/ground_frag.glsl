#version 330 core
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matAmbientColor;
uniform float matShine;
uniform mat4 V;
uniform int terrainMin;
uniform int terrainMax;

uniform sampler2D Grass;
uniform sampler2D Mountain;
uniform sampler2D Snow;

const int NumRegions = 4;
uniform float regions[4];
uniform vec3 regionColors[4];

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
in vec3 vertPos;
in vec3 facePos;
in vec3 modelNor;
in vec3 vertNor;
in vec3 viewNor;

out vec4 color;

//helper
float stepmix(float edge0, float edge1, float E, float x)
{
    float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);
    return mix(edge0, edge1, T);
}

//void setTerrainColor() {
//    if (fragPos.y < 0) {
//        matDiffuseColor = vec3(0, 0, 1);
//        matSpecularColor = vec3(0, 0, 1);
//        matAmbientColor = vec3(0, 0, 1);
//    } else {
//        matDiffuseColor = vec3(0, 1, 0);
//        matSpecularColor = vec3(0, 1, 0);
//        matAmbientColor = vec3(0, 1, 0);
//    }
//}

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
        float distanceToLight = length(lightPos - fragPos);

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
    
//    setTerrainColor();
    vec3 ambientColor = matAmbientColor;
    vec3 diffuseColor = matDiffuseColor;
    vec3 specularColor = matSpecularColor;
//    vec3 modelN = normalize(modelNor);
//    float heightValue = (facePos.y-terrainMin) / (terrainMax-terrainMin);
//    vec3 heightColor = vec3(221/255.0f, 221/255.0f, 228/255.0f);
//    vec3 textureColors[4] = vec3[](
//        texture(Grass, vertPos.xz).xyz,
//        texture(Grass, vertPos.xz).xyz,
//        texture(Mountain, vertPos.xz).xyz,
//        texture(Snow, vertPos.xz).xyz
//    );
//    vec3 textureColor = textureColors[3];
//    
//    for (int i = 0; i < NumRegions; i++) {
//        float height = regions[i];
//        if (heightValue <= height) {
//            if (i <= 1) textureColor = texture(Grass, vertPos.xz).xyz;
//            else if (i == 2) textureColor = texture(Mountain, vertPos.xz).xyz;
//            else textureColor = texture(Snow, vertPos.xz).xyz;
//            if (i > 0) {
//                float prevHeight = regions[i-1];
//                float heightDist = height - prevHeight;
//                float contribution = pow((heightValue - prevHeight) / heightDist, 1);
//                heightColor = (1-contribution) * regionColors[i-1] + contribution * regionColors[i];
//                textureColor = (1-contribution) * textureColors[i-1] + contribution * textureColors[i];
//            } else {
//                heightColor = regionColors[i];
//            }
//            break;
//        }
//    }
//    ambientColor = diffuseColor = specularColor = heightColor / 3.0f;

    //ambient
    vec3 ambient = light.ambientCoefficient * ambientColor * light.intensities;
    
    //diffuse
    vec3 diffuse = df * diffuseColor * max(dot(vertexN, lightN), 0) * light.intensities;
    //vec3 diffuse = matDiffuseColor * df;
    
    //specular
    float alpha = matShine;
    vec3 halfValue = normalize(viewN + lightN);
    vec3 specular = vec3(0);//sf * specularColor * pow(max(dot(vertexN, halfValue), 0), alpha) * light.intensities;
    //vec3 specular = matSpecularColor  * sf;
    
    //linear color (color before gamma correction)
    return ambient + attenuation*(diffuse + specular);
}

//float rand(vec2 co) {
//    highp float a = 12.9898;
//    highp float b = 78.233;
//    highp float c = 43758.5453;
//    highp float dt= dot(co.xy ,vec2(a,b));
//    highp float sn= mod(dt,3.14);
//    return fract(sin(sn) * c);
//}

float rand(vec2 co){
    return fract(sin(dot(co.xy ,vec2(12.9898,78.233))) * 43758.5453);
}

void main() {
    vec3 ambientColor = matAmbientColor;
    vec3 diffuseColor = matDiffuseColor;
    vec3 specularColor = matSpecularColor;
    vec3 modelN = normalize(modelNor);
    float heightValue = (facePos.y-terrainMin) / (terrainMax-terrainMin);
    vec3 heightColor = vec3(221/255.0f, 221/255.0f, 228/255.0f);
    vec3 textureColors[4] = vec3[](
        texture(Grass, vertPos.xz).xyz,
        texture(Grass, vertPos.xz).xyz,
        texture(Mountain, vertPos.xz).xyz,
        texture(Snow, vertPos.xz).xyz
        );
    vec3 textureColor = textureColors[3];

    for (int i = 0; i < NumRegions; i++) {
        float height = regions[i];
        if (heightValue <= height) {
            if (i <= 1) textureColor = texture(Grass, vertPos.xz).xyz;
            else if (i == 2) textureColor = texture(Mountain, vertPos.xz).xyz;
            else textureColor = texture(Snow, vertPos.xz).xyz;
            if (i > 0) {
                float prevHeight = regions[i-1];
                float heightDist = height - prevHeight;
                float contribution = pow((heightValue - prevHeight) / heightDist, 1);
                heightColor = (1-contribution) * regionColors[i-1] + contribution * regionColors[i];
                textureColor = (1-contribution) * textureColors[i-1] + contribution * textureColors[i];
            } else {
                heightColor = regionColors[i];
            }
            break;
        }
    }
    // Make triangles vary in brightness slightly for a more low-poly look
    vec3 randIntensity = rand(facePos.xz) * vec3(0.02, 0.02, 0.02);
//    vec3 finalColor = (diffuseColor + ambientColor);
//    color = vec4(finalColor, 1.0);
//    ambientColor = diffuseColor = vec3(0.5, 0.5, 0.5);
    vec3 finalColor = heightValue * modelN.y * heightColor + randIntensity;
    color = vec4(heightColor*modelN.y + randIntensity, 1.0);
//    color = vec4(textureColor*modelN.y, 1.0);

//    // Normalize the vectors
//    vec3 vertexN = normalize(vertNor);
//    vec3 viewN = normalize(viewNor);
//    //combine color from all the lights
//    vec3 linearColor = vec3(0);
//    for(int i = 0; i < numLights; ++i){
//        vec3 pos = vec3(V * vec4(vec3(allLights[i].position), 1));
//        linearColor += ApplyLight(allLights[i], vertexN, viewN, pos);
//    }
//    
//    float edgeDetection = (dot(viewN, vertexN) > 0.3) ? 1 : 0;
//    
//    //color = vec4(edgeDetection*linearColor, 1.0);
//    color = vec4(linearColor, 1.0);
//    //final color (after gamma correction)
//    //vec3 gamma = vec3(1.0/2.2);
//    //color = vec4(pow(totalPhong, gamma), 1.0);
}
