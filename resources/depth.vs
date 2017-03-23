#version  330 core

layout(location = 0) in vec3 vertPos;
layout(location = 1) in vec3 vertNor;
layout(location = 3) in ivec4 vertID;
layout(location = 4) in vec4 vertWeight;

uniform mat4 LP;
uniform mat4 LV;
uniform mat4 M;

const int MAX_BONES = 100;
uniform mat4 Bones[MAX_BONES];
uniform bool hasBones;

void main() {
    if (hasBones) {
        mat4 BMatrix = Bones[vertID[0]] * vertWeight[0];
        BMatrix += Bones[vertID[1]] * vertWeight[1];
        BMatrix += Bones[vertID[2]] * vertWeight[2];
        BMatrix += Bones[vertID[3]] * vertWeight[3];
        gl_Position = LP * LV * M * BMatrix * vec4(vertPos.xyz, 1.0);
    } else {
        /* transform into light space */
        gl_Position = LP * LV * M * vec4(vertPos.xyz, 1.0);
    }
}
