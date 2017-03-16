#version 330 core

layout(location = 0) in vec3 squareVertices;
layout(location = 1) in vec3 vertPos; // Position of the center of the particle and size of the square
layout(location = 2) in vec4 color;

uniform float scale;
// Output data ; will be interpolated for each fragment.
out vec2 UV;
out vec4 particle_color;

// Values that stay constant for the whole mesh.
uniform vec3 CameraRight_worldspace;
uniform vec3 CameraUp_worldspace;
uniform mat4 P;
uniform mat4 V;

void main()
{
	float particleSize = scale;
	vec3 particleCenter_worldspace = vertPos.xyz;
	
	// Billboarding
	vec3 vertexPosition_worldspace = particleCenter_worldspace 
	    + CameraRight_worldspace * squareVertices.x * particleSize
		+ CameraUp_worldspace * squareVertices.y * particleSize;

	// Output position of the vertex
	gl_Position = P * V * vec4(vertexPosition_worldspace, 1.0f);

	// UV of the vertex
	UV = squareVertices.xy + vec2(0.5, 0.5);
	particle_color = color;
}