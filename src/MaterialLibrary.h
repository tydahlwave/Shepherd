//
//  MaterialLibrary.h
//  Shepherd
//
//  Created by Tyler Dahl on 2/9/17.
//
//

#ifndef MaterialLibrary_h
#define MaterialLibrary_h

#include "Material.h"

class MaterialLibrary {
public:
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

#endif /* MaterialLibrary_h */
