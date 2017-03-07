#version 330 core

out vec4 Outcolor;
uniform int width;
uniform int height;

void main() {
    float dist = sqrt(pow(gl_FragCoord.x-width/2, 2) + pow(gl_FragCoord.y-height/2, 2));
    if ((dist < 20 && dist > 18) || dist < 3) {
        Outcolor = vec4(1, 0, 0, 1);
    } else {
        discard;
    }
}
