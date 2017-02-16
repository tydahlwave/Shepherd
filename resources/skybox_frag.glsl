#version 330 core
uniform samplerCube skybox;

in vec3 vTexCoord;

out vec4 color;

void main() {
    color = texture(skybox, vTexCoord);
}
