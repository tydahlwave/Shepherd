//
//  MeshRenderer.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef MeshRenderer_h
#define MeshRenderer_h

#include "Component.h"
#include "../Mesh.h"
#include "../Material.h"
#include "../Shader.h"
#include "../Texture.h"

class MeshRenderer : public Component {
public:
    MeshRenderer() { name = "MeshRenderer"; };
    MeshRenderer(Mesh *m, Shader *s, Material *mat, Texture *t) :MeshRenderer() { mesh = m; shader = s; material = mat; texture = t; }
    virtual ~MeshRenderer() {};
    
    Mesh *mesh = nullptr;
    Shader *shader = nullptr;
    Material *material = nullptr;
    Texture *texture = nullptr;
};

#endif /* MeshRenderer_h */
