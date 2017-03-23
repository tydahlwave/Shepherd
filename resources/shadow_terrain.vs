#version 330
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in float vertTex;
uniform mat4 P;
uniform mat4 V;
uniform mat4 LP1;
uniform mat4 LP2;
uniform mat4 LP3;
uniform mat4 LV;
uniform mat4 M;
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matAmbientColor;
uniform float matShine;

uniform vec3 lightDir;

out VS_OUT {
    vec3 fragPos;
    vec3 vertPos;
    vec3 modelPos;
    vec3 modelNor;
    vec3 vertNor;
    vec3 viewNor;
    float vertTex;
    vec3 vColor;
    vec4 fPosLS1;
    vec4 fPosLS2;
    vec4 fPosLS3;
} vs_out;


void main() {
    gl_Position = P * V * M * vertPos;
    
    
    // Pass vertex position and normal to fragment shader
    vs_out.fragPos = (V * M * vertPos).xyz;
    vs_out.vertPos = (M * vertPos).xyz;
    vs_out.modelPos = (M * vertPos).xyz;
    vs_out.modelNor = (M * vec4(vertNor, 0.0)).xyz;
    vs_out.vertNor = (V * M * vec4(vertNor, 0.0)).xyz;
    vs_out.viewNor = -(V * M * vertPos).xyz;
    vs_out.vertTex = vertTex;
    
    /* The vertex in light space */
    vs_out.fPosLS1 = LP1 * LV * M * vec4(vertPos.xyz, 1.0);
    vs_out.fPosLS2 = LP2 * LV * M * vec4(vertPos.xyz, 1.0);
    vs_out.fPosLS3 = LP3 * LV * M * vec4(vertPos.xyz, 1.0);
    /* a color that could be blended - or be shading */
    vs_out.vColor = vec3(max(dot(vs_out.modelNor, normalize(lightDir)), 0));
}
