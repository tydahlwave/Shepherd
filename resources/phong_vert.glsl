#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;

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
