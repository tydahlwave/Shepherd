#version 330 core
uniform sampler2D Texture;

in vec2 vTexCoord;
in vec3 vColor;

out vec4 Outcolor;

void main() {
  vec4 BaseColor = vec4(vColor.x, vColor.y, vColor.z, 1);
  vec4 texColor = texture(Texture, vTexCoord);
 
  Outcolor = vec4(texColor.r*vColor.r, texColor.g*vColor.g, texColor.b*vColor.b, 1);
  //DEBUG:Outcolor = vec4(vTexCoord.s, vTexCoord.t, 0, 1);
}

