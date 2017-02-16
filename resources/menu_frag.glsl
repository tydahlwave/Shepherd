#version 330 core
out vec4 color;

in vec3 vertexNormal;
in vec3 viewNormal;
in vec3 col;

void main()
{

    color = vec4(col, 1.0);
}
