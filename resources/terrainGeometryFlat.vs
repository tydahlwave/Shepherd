#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 fragPos;
out vec3 vertPos;
out vec3 facePos;
out vec3 modelNor;
out vec3 vertNor;
out vec3 viewNor;
out vec3 modelPos;
out vec3 modelNor;

in VS_OUT {
    vec3 fragPos;
    vec3 vertPos;
    vec3 modelNor;
    vec3 vertNor;
    vec3 viewNor;
    vec3 modelPos;
    vec3 modelNor;
} gs_in[];


void makeTriangle() {
    vec3 facePosition = (gs_in[0].vertPos + gs_in[1].vertPos + gs_in[2].vertPos) / 3;
    vec3 faceNormal = (gs_in[0].vertNor + gs_in[1].vertNor + gs_in[2].vertNor) / 3;
    vec3 faceModelNormal = (gs_in[0].modelNor + gs_in[1].modelNor + gs_in[2].modelNor) / 3;
    for (int i = 0; i < 3; i++) {
        gl_Position = gl_in[i].gl_Position;
        fragPos = gs_in[i].fragPos;
        vertPos = gs_in[i].vertPos;
        facePos = facePosition;
        modelNor = gs_in[i].modelNor;
//        modelNor = faceModelNormal;
        vertNor = faceNormal;
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
