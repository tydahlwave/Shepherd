#version 330 core

in float chargeFrag;

out vec4 Outcolor;

void main() {
    if((gl_FragCoord.x < 200) && (gl_FragCoord.y < 500 * chargeFrag)){
        Outcolor = vec4(1, 0, 0, 1);
    } else {
        discard;
    }
}
