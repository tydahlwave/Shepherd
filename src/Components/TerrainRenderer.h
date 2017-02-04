//
//  TerrainRenderer.h
//  Shepherd
//
//  Created by Tyler Dahl on 2/3/17.
//
//

#ifndef TerrainRenderer_h
#define TerrainRenderer_h

#include "Component.h"
#include "../Material.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../Terrain.h"

class TerrainRenderer : public Component {
public:
    TerrainRenderer() { name = "TerrainRenderer"; };
    TerrainRenderer(Terrain *t, Shader *s, Material *mat, Texture *tex) :TerrainRenderer() { terrain = t; shader = s; material = mat; texture = tex; }
    virtual ~TerrainRenderer() {};
    
    Terrain *terrain = nullptr;
    Shader *shader = nullptr;
    Material *material = nullptr;
    Texture *texture = nullptr;
};

#endif /* TerrainRenderer_h */
