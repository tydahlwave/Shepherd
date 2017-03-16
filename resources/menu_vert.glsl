//Phong Reflection Model Vertex Shader
#version 330
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;

//send them to fragment shader

out vec3 fragPos;
out vec3 fragNor;

out vec3 vertexNormal;
out vec3 viewNormal;
out vec3 col;

out vec3 modelPos;
out vec3 modelNor;

void main()
{
    
    gl_Position = P * V * M * vertPos;
    
    float offset = vertPos.z*1.5;
    if(vertPos.z > 0) {
        col = vec3(0.6,0.3,0.1) + vec3(offset);
    }
    else {
        col = vec3(0.5,0.5,0) - vec3(offset*2.0);
    }
    
    // Normalize the vectors
    vertexNormal = (V * M * vec4(vertNor, 0.0)).xyz;
    viewNormal = -(V * M * vertPos).xyz;
    
    // Pass vertex position and normal to fragment shader
    fragPos = (V * M * vertPos).xyz;
    fragNor = (V * M * vec4(vertNor, 0.0)).xyz;
    
    modelPos = (M * vertPos).xyz;
    modelNor = (M * vec4(vertNor, 0)).xyz;
    
    
}
