//Toon Model Vertex Shader
#version 330
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 3) in ivec4 vertID;
layout(location = 4) in vec4 vertWeight;
layout(location = 5) in vec2 vertTex;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matAmbientColor;
uniform float matShine;

const int MAX_BONES = 100;
uniform mat4 Bones[MAX_BONES];

//send them to fragment shader

out vec3 fragPos;
out vec3 fragNor;

out vec3 vertexNormal;
out vec3 viewNormal;

//send them to fragment shader
out VS_OUT {
    vec3 fragPos;
    vec3 vertPos;
    vec3 modelNor;
    vec3 vertNor;
    vec3 viewNor;
    vec3 modelPos;
    vec2 vertTex;
} vs_out;


void main()
{
    mat4 BMatrix = Bones[vertID[0]] * vertWeight[0];
    BMatrix += Bones[vertID[1]] * vertWeight[1];
    BMatrix += Bones[vertID[2]] * vertWeight[2];
    BMatrix += Bones[vertID[3]] * vertWeight[3];
    gl_Position = P * V * M * BMatrix * vertPos;
    //gl_Position = P * V * M * vertPos;
    
    // Pass vertex position and normal to fragment shader
    vs_out.fragPos = (V * M * vertPos).xyz;
    vs_out.vertPos = (M * vertPos).xyz;
    vs_out.modelNor = (M * vec4(vertNor, 0.0)).xyz;
    vs_out.vertNor = (V * M * vec4(vertNor, 0.0)).xyz;
    vs_out.viewNor = -(V * M * vertPos).xyz;
    
    vs_out.modelPos = (M * vertPos).xyz;
    vs_out.vertTex = vertTex;
    
}
