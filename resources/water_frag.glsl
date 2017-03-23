#version 330 core 

uniform sampler2D reflectionTexture;
uniform sampler2D refractionTexture;
uniform sampler2D dudvMap;
uniform sampler2D normalMap;
uniform vec3 lightColor;

in vec2 textureCoords;
in vec4 clipSpace;
in vec3 toCameraVector;
in vec3 fromLightVector;

out vec4 color;

uniform float moveFactor;

const float waveStrength = 0.01;
const float shineDamper = 60.0;
const float reflectivity = 0.3;

void main() {

   vec2 ndc = (clipSpace.xy / clipSpace.w)/2.0 + 0.5;
   vec2 refractTexCoords = vec2(ndc.x, ndc.y);
   vec2 reflectTexCoords = vec2(ndc.x, -ndc.y);

   vec2 distortedTexCoords = texture(dudvMap, vec2(textureCoords.x + moveFactor, textureCoords.y)).rg * 0.1;
   distortedTexCoords = textureCoords + vec2(distortedTexCoords.x, distortedTexCoords.y + moveFactor);
   vec2 total = (texture(dudvMap, distortedTexCoords).rg * 2.0 - 1.0) * waveStrength;

   refractTexCoords += total;
   refractTexCoords = clamp(refractTexCoords, 0.001, 0.999);
   reflectTexCoords += total;
   reflectTexCoords.x = clamp(reflectTexCoords.x, 0.001, 0.999);
   reflectTexCoords.y = clamp(reflectTexCoords.y, -0.999, 0.001);

   vec4 reflectColor = texture(reflectionTexture, reflectTexCoords);
   vec4 refractColor = texture(refractionTexture, refractTexCoords);

   //Fresnel Effect
   vec3 viewVector = normalize(toCameraVector);
   float refractiveFactor = dot(viewVector, vec3(0, 1, 0));
   refractiveFactor = pow(refractiveFactor, 0.75);

   vec4 normalMapColor = texture(normalMap, distortedTexCoords);
   vec3 normal = vec3(normalMapColor.r * 2.0 - 1.0, normalMapColor.b, normalMapColor.g * 2.0 - 1.0);
   normal = normalize(normal);

   vec3 reflectedLight = reflect(normalize(fromLightVector), normal);
   float specular = max(dot(reflectedLight, viewVector), 0.0);
   specular = pow(specular, shineDamper);
   vec3 specularHighlights = lightColor * specular * reflectivity;

   color = mix(reflectColor, refractColor, refractiveFactor);
   //color = mix(color, vec4(0.1, 0.69, 1.0, 0.8), 0.5) + vec4(specularHighlights, 0.0);
      color = mix(color, vec4(0.5, 0.83, 1.0, 0.9), 0.5) + vec4(specularHighlights, 0.0);

}
