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
#include "../Model.h"
#include "../Material.h"
#include "../Shader.h"
#include "../Texture.h"

class MeshRenderer : public Component {
public:
    MeshRenderer() { name = "MeshRenderer"; };
    MeshRenderer(Model *m, Shader *s, Material *mat, Texture *t) :MeshRenderer() { model = m; shader = s; material = mat; texture = t; }
    
    Model *model = nullptr;
    Shader *shader = nullptr;
    Material *material = nullptr;
    Texture *texture = nullptr;
};

#endif /* MeshRenderer_h */
