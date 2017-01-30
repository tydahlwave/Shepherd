#version 330 core

out vec4 Outcolor;

void main() {
    float dist = sqrt(pow(gl_FragCoord.x-640, 2) + pow(gl_FragCoord.y-480, 2));
    if ((dist < 20 && dist > 18) || dist < 3) {
        Outcolor = vec4(1, 0, 0, 1);
    } else {
        discard;
    }
}
