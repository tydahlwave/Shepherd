//Toon Shading Fragment Shader
#version 330
layout(location = 0) out vec4 color;



in vec3 fragPos;
in vec3 fragNor;
in vec3 fragLightPos;
in vec3 fragLightColor;
in vec3 fragSunDir;
in vec3 fragSunColor;
in vec3 fragDiffuseColor;
in vec3 fragSpecularColor;
in vec3 fragAmbientColor;
in float fragShine;


in vec3 vertexNormal;
in vec3 lightNormal;
in vec3 viewNormal;
in vec3 sunVertexNormal;


const vec3 ambinetColor = vec3(0.90,0.0,0.20);

//number of levels
//for diffuse color
const int levels = 5;
const float scaleFactor = 1.0 / levels;

//helper
float stepmix(float edge0, float edge1, float E, float x)
{
    float T = clamp(0.5 * (x - edge0 + E) / E, 0.0, 1.0);
    return mix(edge0, edge1, T);
}


void main()
{
    
    
    // Normalize the vectors
    vec3 vertexN = normalize(vertexNormal);
    vec3 lightN = normalize(lightNormal);
    vec3 viewN = normalize(viewNormal);
    vec3 sunVertexN = normalize(sunVertexNormal);
   
   
    float specMask = pow(max(dot(vertexN, normalize(vertexN + lightN)), 0.0), fragShine);
    float diffMask = (dot(vertexN + lightN, fragNor) < 0.4) ? 1 : 0;
    
    vec3 L = normalize(fragLightPos);

    const float A = 0.1;
    const float B = 0.2;
    const float C = 0.4;
    const float D = 0.6;
    const float E = 0.8;
    const float F = 1.0;
    
    float df = max(0.0, dot(vertexN, L));
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
    
    float sf = max(0.0, dot(vertexN, normalize(L + vec3(0,0,1))));
    
    sf = pow(sf, fragShine);
    
    sf = step(0.5, sf);
   
    // Calculate diffuse color
    //vec3 diffuse = (fragDiffuseColor * max(dot(vertexN, lightN), 0) * fragLightColor) * df;
    vec3 diffuse = fragDiffuseColor * df;
    // Calculate specular color
    float alpha = fragShine;
    vec3 halfValue = normalize(viewN + lightN);
    //vec3 specular = (fragSpecularColor * pow(max(dot(vertexN, halfValue), 0), alpha) * fragLightColor) * sf;
    vec3 specular = fragSpecularColor  * sf;
    // Calculate ambient color
    vec3 ambient = fragAmbientColor;
    
    
    // Calculate distance attenuation
    float a = 0;
    float b = 0.5;
    float c = 0;
    float distanceToLight = length(fragLightPos - fragPos);
    float distanceAttenuation = 1 / (a + b * distanceToLight + c * pow(distanceToLight, 2));
    
    // Calculate phong from lights
    vec3 lightsPhong = distanceAttenuation * (diffuse + specular);
    
    // Calculate phong from sun
    vec3 sunDir = normalize(fragSunDir);
    vec3 sunDiffuse = fragDiffuseColor * max(dot(sunVertexN, sunDir), 0) * fragSunColor;
    halfValue = normalize(viewN + sunDir);
    vec3 sunSpecular = fragSpecularColor * pow(max(dot(sunVertexN, halfValue), 0), alpha) * fragSunColor;
    // No distance attenuation, but the vector magnitude affects the brightness
    vec3 sunPhong = length(fragSunDir) * (sunDiffuse * df + sunSpecular * sf);
    
    
    float edgeDetection = (dot(viewN, vertexN) > 0.3) ? 1 : 0;

    // This provides a cool effect if I use it instead of the phong calculation
    vec3 sunLight = max(dot(sunVertexN, normalize(fragSunDir)), 0) * fragSunColor;
    
    //vec3 totalPhong = edgeDetection * (ambient + (lightsPhong * specMask) + sunPhong);
    
    vec3 totalPhong = edgeDetection * (ambient + lightsPhong + sunPhong);
    
    
    color = vec4(totalPhong, 1.0);
}
