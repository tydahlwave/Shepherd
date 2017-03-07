#version  330 core

layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
out vec2 pos;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;

void main() {
    gl_Position = P * V * M * vertPos;
    pos = (vertPos.xy + 1) / 2;
    // This is useless. It's here to silence runtime warnings.
    mat4 test = P * V * M;
    vec4 test2 = vec4(vertNor, 0);
}
