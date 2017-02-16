//Phong Reflection Model Vertex Shader
#version 330
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 sunDir;
uniform vec3 sunColor;
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matAmbientColor;
uniform float matShine;

//send them to fragment shader

out vec3 fragPos;
out vec3 fragNor;

out vec3 vertexNormal;
out vec3 viewNormal;

void main()
{
    
    gl_Position = P * V * M * vertPos;
    
    // Normalize the vectors
    vertexNormal = (V * M * vec4(vertNor, 0.0)).xyz;
    viewNormal = -(V * M * vertPos).xyz;
    
    // Pass vertex position and normal to fragment shader
    fragPos = (V * M * vertPos).xyz;
    fragNor = (V * M * vec4(vertNor, 0.0)).xyz;

}
