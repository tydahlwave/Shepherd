#version 330 core 
in vec3 fragPos;
in vec3 fragNor;
in vec3 fragLightPos;
in vec3 fragLightColor;
in vec3 fragSunDir;
in vec3 fragSunColor;
in vec3 fragDiffuseColor;
in vec3 fragSpecularColor;
in vec3 fragAmbientColor;
in float fragShine;
out vec4 color;

in vec3 vertexNormal;
in vec3 lightNormal;
in vec3 viewNormal;
in vec3 sunVertexNormal;

void main()
{
   // Normalize the vectors
   vec3 vertexN = normalize(vertexNormal);
   vec3 lightN = normalize(lightNormal);
   vec3 viewN = normalize(viewNormal);
   vec3 sunVertexN = normalize(sunVertexNormal);

   // Calculate diffuse color
   vec3 diffuse = fragDiffuseColor * max(dot(vertexN, lightN), 0) * fragLightColor;

   // Calculate specular color
   float alpha = fragShine;
   vec3 halfValue = normalize(viewN + lightN);
   vec3 specular = fragSpecularColor * pow(max(dot(vertexN, halfValue), 0), alpha) * fragLightColor;

   // Calculate ambient color
   vec3 ambient = fragAmbientColor;

   // Calculate distance attenuation
   float a = 0;
   float b = 0.5;
   float c = 0;
   float distanceToLight = length(fragLightPos - fragPos);
   float distanceAttenuation = 1 / (a + b * distanceToLight + c * pow(distanceToLight, 2));

   // Calculate phong from lights
   vec3 lightsPhong = distanceAttenuation * (diffuse + specular);

   // Calculate phong from sun
   vec3 sunDir = normalize(fragSunDir);
   vec3 sunDiffuse = fragDiffuseColor * max(dot(sunVertexN, sunDir), 0) * fragSunColor;
   halfValue = normalize(viewN + sunDir);
   vec3 sunSpecular = fragSpecularColor * pow(max(dot(sunVertexN, halfValue), 0), alpha) * fragSunColor;
   // No distance attenuation, but the vector magnitude affects the brightness
   vec3 sunPhong = length(fragSunDir) * (sunDiffuse + sunSpecular);

   // This provides a cool effect if I use it instead of the phong calculation
   // vec3 sunLight = max(dot(sunVertexN, normalize(fragSunDir)), 0) * fragSunColor;

   vec3 totalPhong = ambient + lightsPhong + sunPhong;

   color = vec4(totalPhong, 1.0);
}
