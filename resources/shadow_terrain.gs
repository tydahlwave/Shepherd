#version 330 core
layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

out vec3 fragPos;
out vec3 vertPos;
out vec3 facePos;
out vec3 modelPos;
out vec3 modelNor;
out vec3 vertNor;
out vec3 viewNor;
out float vertTex;
out vec3 vColor;
out vec4 fPosLS1;
out vec4 fPosLS2;
out vec4 fPosLS3;

in VS_OUT {
    vec3 fragPos;
    vec3 vertPos;
    vec3 modelPos;
    vec3 modelNor;
    vec3 vertNor;
    vec3 viewNor;
    float vertTex;
    vec3 vColor;
    vec4 fPosLS1;
    vec4 fPosLS2;
    vec4 fPosLS3;
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
        modelPos = gs_in[i].modelPos;
        modelNor = gs_in[i].modelNor;
        //        modelNor = faceModelNormal;
        vertNor = faceNormal;
        viewNor = gs_in[i].viewNor;
        vertTex = gs_in[i].vertTex;
        vColor = gs_in[i].vColor;
        fPosLS1 = gs_in[i].fPosLS1;
        fPosLS2 = gs_in[i].fPosLS2;
        fPosLS3 = gs_in[i].fPosLS3;
        EmitVertex();
    }
    EndPrimitive();
}

void main() {
    makeTriangle();
}
