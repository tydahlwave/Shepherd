#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 fragPos;
out vec3 fragNor;
out vec3 viewNor;
out vec3 modelPos;
out vec3 modelNor;

in VS_OUT {
    vec3 fragPos;
    vec3 fragNor;
    vec3 viewNor;
    vec3 modelPos;
    vec3 modelNor;
} gs_in[];


void makeTriangle() {
    vec3 faceNormal = (gs_in[0].fragNor + gs_in[1].fragNor + gs_in[2].fragNor) / 3;
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        fragPos = gs_in[i].fragPos;
        fragNor = faceNormal;
        viewNor = gs_in[i].viewNor;
        modelPos = gs_in[i].modelPos;
        modelNor = gs_in[i].modelNor;
        EmitVertex();
    }
    EndPrimitive();
}

void main() {
    makeTriangle();
}
