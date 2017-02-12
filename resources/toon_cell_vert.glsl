//Phong Reflection Model Vertex Shader
#version 330

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;

uniform mat4 P;
uniform mat4 M;
uniform mat4 V;

//send them to fragment shader
out vec3 world_pos;
out vec3 world_normal;

void main()
{
    
    //convert in world coords
    world_pos = mat3(M) * vertPos;//careful here
    world_normal = normalize(mat3(M) * vertNor);
    gl_Position = P * M * V * vec4(vertPos,1);
}
