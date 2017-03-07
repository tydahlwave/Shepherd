#version 330 core

out vec4 Outcolor;
uniform sampler2D ButtonTexture;
in vec2 pos;

void main() {
    Outcolor = texture(ButtonTexture, pos);
}
