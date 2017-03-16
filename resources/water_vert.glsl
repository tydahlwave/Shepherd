#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 cameraPos;
uniform vec3 lightPos;
//uniform vec4 plane;

out vec2 textureCoords;
out vec4 clipSpace;
out vec3 toCameraVector;
out vec3 fromLightVector;

const float tiling = 6.0;

void main()
{
   vec4 worldPos = M * vertPos;
   clipSpace = P * V * worldPos;
   gl_Position = clipSpace;
   textureCoords = vec2(vertPos.x/2.0 + 0.5, vertPos.y/2.0 + 0.5) * tiling;
   toCameraVector = cameraPos - worldPos.xyz;
   fromLightVector = worldPos.xyz - lightPos;
}
