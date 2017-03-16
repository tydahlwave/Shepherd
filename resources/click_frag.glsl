#version 330 core

out vec4 Outcolor;
uniform vec4 PickingColor;


void main() {
    Outcolor = PickingColor;
}
