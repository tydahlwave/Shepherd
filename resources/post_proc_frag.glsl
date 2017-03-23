#version 330 core

in vec2 texCoord;
out vec4 color;
uniform sampler2D texBuf;
uniform float offset;

/* just pass through the texture color but add a little blue to anything on left side */
void main(){
    vec2 texcoord = texCoord;
    texcoord.x += sin(texcoord.y * 4*2*3.14159 + offset) / 100;
    color = texture(texBuf, texcoord);
}
