#version 330
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 2) in float vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matAmbientColor;
uniform float matShine;

out VS_OUT {
    vec3 fragPos;
    vec3 vertPos;
    vec3 modelPos;
    vec3 modelNor;
    vec3 vertNor;
    vec3 viewNor;
    float vertTex;
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
}
