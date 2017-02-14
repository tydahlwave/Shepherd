//
//  Material.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Material_h
#define Material_h

#include "Color.h"

class Material {
public:
    Material() {};
    Material(Color *c) { ambientColor = c; diffuseColor = c; specularColor = c; };
    Material(Color *a, Color *d, Color *s, float sh) { ambientColor = a; diffuseColor = d; specularColor = s; shine = sh; };
    virtual ~Material() {};
    
    Color *ambientColor = nullptr;
    Color *diffuseColor = nullptr;
    Color *specularColor = nullptr;
    float shine = 50.0f;
    
    static void InitializeMaterials();
    static Material *shinyBluePlastic;
    static Material *pearl;
    static Material *brass;
    static Material *copper;
    static Material *bronze;
    static Material *ruby;
    static Material *emerald;
    static Material *polishedGold;
    static Material *grass;
};

#endif /* Material_h */
