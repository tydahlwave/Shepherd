//Toon Model Vertex Shader
#version 330
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 3) in ivec4 vertID;
layout(location = 4) in vec4 vertWeight;
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

out vec4 we;
out vec4 id;

void main()
{
    
    mat4 BMatrix = Bones[vertID[0]] * vertWeight[0];
    BMatrix += Bones[vertID[1]] * vertWeight[1];
    BMatrix += Bones[vertID[2]] * vertWeight[2];
    BMatrix += Bones[vertID[3]] * vertWeight[3];
    
    if(vertID[0] < 0 || vertWeight[0] > 1)
        gl_Position = P * V * M * vertPos;
    else{
         gl_Position = P * V * M * BMatrix * vertPos;
    }
    
    //coords0 = s_vCoords0.xy;
   
    //gl_Position = P * V * M * vertPos;
    
    // Normalize the vectors
    vertexNormal = (V * M * vec4(vertNor, 0.0)).xyz;
    viewNormal = -(V * M * vertPos).xyz;
    
    // Pass vertex position and normal to fragment shader
    fragPos = (V * M * vertPos).xyz;
    fragNor = (V * M * vec4(vertNor, 0.0)).xyz;
    
    we = vertWeight;
    id = vertID;
    
}
