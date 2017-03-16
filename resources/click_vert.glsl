#version  330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec4 PickingColor;

void main() {
    gl_Position = P * V * M * vertPos;
    
    // This is useless. It's here to silence runtime warnings.
    mat4 test = P * V;
    vec4 test2 = vec4(vertNor, 0);
}
