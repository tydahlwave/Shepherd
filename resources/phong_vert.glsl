#version 330 core
layout(location = 0) in vec4 vertPos;
layout(location = 1) in vec3 vertNor;
uniform mat4 P;
uniform mat4 M;
uniform mat4 V;
uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 sunDir;
uniform vec3 sunColor;
uniform vec3 matDiffuseColor;
uniform vec3 matSpecularColor;
uniform vec3 matAmbientColor;
uniform float matShine;

out vec3 fragPos;
out vec3 fragNor;

out vec3 fragLightPos;
out vec3 fragLightColor;
out vec3 fragSunDir;
out vec3 fragSunColor;
out vec3 fragDiffuseColor;
out vec3 fragSpecularColor;
out vec3 fragAmbientColor;
out float fragShine;

out vec3 vertexNormal;
out vec3 lightNormal;
out vec3 viewNormal;
out vec3 sunVertexNormal;

void main()
{
   gl_Position = P * V * M * vertPos;

   // Normalize the vectors
   vertexNormal = (V * M * vec4(vertNor, 0.0)).xyz;
   lightNormal = lightPos - (V * M * vertPos).xyz;
   viewNormal = -(V * M * vertPos).xyz;
   sunVertexNormal = (M * vec4(vertNor, 0.0)).xyz;

   // Pass vertex position and normal to fragment shader
   fragPos = (V * M * vertPos).xyz;
   fragNor = (V * M * vec4(vertNor, 0.0)).xyz;

   // Pass light pos to fragment shader
   fragLightPos = lightPos;
   fragSunDir = sunDir;

   // Pass colors to fragment shader
   fragLightColor = lightColor;
   fragSunColor = sunColor;
   fragDiffuseColor = matDiffuseColor;
   fragSpecularColor = matSpecularColor;
   fragAmbientColor = matAmbientColor;
   fragShine = matShine;
}
