//
//  TerrainRenderer.h
//  Shepherd
//
//  Created by Tyler Dahl on 2/3/17.
//
//

#ifndef TerrainRenderer_h
#define TerrainRenderer_h

#include "glm/glm.hpp"

#include "Component.h"
#include "../Material.h"
#include "../Shader.h"
#include "../Texture.h"
#include "../Terrain.h"

class TerrainRenderer : public Component {
public:
    TerrainRenderer() { name = "TerrainRenderer"; };
    TerrainRenderer(Terrain *t, Shader *s, Material *mat, std::vector<Texture*> texs) :TerrainRenderer() { terrain = t; shader = s; material = mat; textures = texs; }
    virtual ~TerrainRenderer() {};
    
    Terrain *terrain = nullptr;
    Shader *shader = nullptr;
    Material *material = nullptr;
    std::vector<Texture*> textures;
    
    std::vector<float> regions = {
    //    0,      // Water
    //    0.02,   // Sand
        0.02f,   // Grass
        0.05f,    // Light grass
        0.2f,    // Rock
        0.5f     // Snow
    };
    std::vector<glm::vec3> regionColors = {
    //    glm::vec3(68/255.0f, 68/255.0f, 122/255.0f),     // Water
    //    glm::vec3(210/255.0f, 185/255.0f, 139/255.0f),   // Sand
        glm::vec3(85/255.0f, 153/255.0f, 68/255.0f),     // Grass
//        glm::vec3(136/255.0f, 153/255.0f, 119/255.0f),   // Light grass
        glm::vec3(136/255.0f, 136/255.0f, 136/255.0f),   // Rock
        glm::vec3(255/255.0f, 255/255.0f, 255/255.0f)    // Snow
    //    glm::vec3(221/255.0f, 221/255.0f, 228/255.0f)    // Snow
    };
    
//    void AddTexture(Texture *texture) { textures.push_back(texture); }
    
    void Serialize(rapidjson::Writer<rapidjson::StringBuffer> &writer) {
        
    }
};

#endif /* TerrainRenderer_h */
