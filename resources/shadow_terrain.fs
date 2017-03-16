#version 330 core
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matAmbientColor;
uniform float matShine;
uniform mat4 V;
uniform int terrainMin;
uniform int terrainMax;
uniform vec3 terrainScale;

uniform sampler2D Grass;
uniform sampler2D Mountain;
uniform sampler2D Snow;
uniform sampler2D shadowDepth;

uniform bool useTextureMap;
uniform bool useTextures;

const int NumRegions = 3;
uniform float regions[NumRegions];
uniform vec3 regionColors[NumRegions];

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
in vec3 modelPos;
in vec3 modelNor;
in vec3 vertNor;
in vec3 viewNor;
in float vertTex;
in vec3 vColor;
in vec4 fPosLS;

out vec4 color;

float TestShadow() {
    float bias = 0.01;
    
    //1: shift the coordinates from -1, 1 to 0 ,1
    vec4 shifted = (fPosLS + 1)/2.0f;
    //2: read off the stored depth (.) from the ShadowDepth, using the shifted.xy
    float depth = texture(shadowDepth, shifted.xy).r;
    //3: compare to the current depth (.z) of the projected depth
    if (shifted.z - bias > depth) {
        //4: return 1 if the point is shadowed
        return 1.0f;
    }
    
    return 0.0;
}

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

vec4 getColor() {
    vec3 modelN = normalize(modelNor);
    float heightValue = (facePos.y/terrainScale.y-terrainMin) / (terrainMax-terrainMin);
    vec3 textureColors[NumRegions] = vec3[](
                                            texture(Grass, vertPos.xz / 3).xyz,
                                            texture(Mountain, vertPos.xz / 3).xyz,
                                            texture(Snow, vertPos.xz / 3).xyz
                                            );
    vec3 textureColor = textureColors[NumRegions-1];
    vec3 heightColor = regionColors[NumRegions-1];
    
    if (useTextureMap) {
        float contribution = fract(vertTex);
        textureColor = textureColors[int(vertTex)] * (1-contribution) + textureColors[min(int(vertTex)+1, NumRegions-1)] * contribution;
        heightColor = regionColors[int(vertTex)] * (1-contribution) + regionColors[min(int(vertTex)+1, NumRegions-1)] * contribution;
    } else {
        for (int i = 0; i < NumRegions; i++) {
            float height = regions[i];
            if (heightValue <= height) {
                textureColor = textureColors[i];
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
    }
    // Make triangles vary in brightness slightly for a more low-poly look
    vec3 randIntensity = rand(facePos.xz) * vec3(0.02, 0.02, 0.02);
    //    vec3 finalColor = (diffuseColor + ambientColor);
    //    color = vec4(finalColor, 1.0);
    //    ambientColor = diffuseColor = vec3(0.5, 0.5, 0.5);
    vec3 finalColor = heightValue * modelN.y * heightColor + randIntensity;
    //    color = vec4(heightColor*modelN.y + randIntensity, 1.0);
    //    color = vec4(textureColor*modelN.y, 1.0);
    
    float Shade = TestShadow();
    
    
    //1: shift the coordinates from -1, 1 to 0 ,1
    vec4 shifted = (fPosLS + 1)/2.0f;
    //2: read off the stored depth (.) from the ShadowDepth, using the shifted.xy
    float depth = texture(shadowDepth, shifted.xy).r;
    
//    return vec4(0, fPosLS.z, fPosLS.z, 1.0);
    return depth * vec4(1);
    if (useTextures) {
        return (1.0-Shade)*vec4(1);//*vec4(textureColor*modelN.y, 1.0);
    } else {
        return (1.0-Shade)*vec4(1);//*vec4(heightColor*modelN.y + randIntensity, 1.0);
    }
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
    
    //    setTerrainColor();
    vec3 ambientColor = matAmbientColor;
    vec3 diffuseColor = matDiffuseColor;
    vec3 specularColor = matSpecularColor;
    ambientColor = diffuseColor = specularColor = getColor().xyz / 2.0f;
    
    //ambient
    vec3 ambient = light.ambientCoefficient * ambientColor * light.intensities;
    
    //diffuse
    vec3 diffuse = df * diffuseColor * max(dot(vertexN, lightN), 0) * light.intensities;
    //vec3 diffuse = matDiffuseColor * df;
    
    //specular
    vec3 specular;
    //    float alpha = matShine;
    //    vec3 halfValue = normalize(viewN + lightN);
    //    vec3 specular = sf * matSpecularColor * pow(max(dot(vertexN, halfValue), 0), alpha) * light.intensities;
    //specular = matSpecularColor  * sf;
    specular = vec3(0);
    
    //linear color (color before gamma correction)
    return ambient + attenuation*(diffuse + specular);
}

void main() {
        color = getColor();
    
//    // Normalize the vectors
//    vec3 vertexN = normalize(modelNor);
//    vec3 viewN = normalize(viewNor);
//    //combine color from all the lights
//    vec3 linearColor = vec3(0);
//    for(int i = 0; i < numLights; ++i){
//        vec3 pos = vec3(V * vec4(vec3(allLights[i].position), 1));
//        linearColor += ApplyLight(allLights[i], vertexN, viewN, pos);
//    }
//    
//    float edgeDetection = (dot(viewN, vertexN) > 0.3) ? 1 : 0;
//    color = vec4(linearColor, 1.0);
    
}
