//
//  LevelEditor.h
//  Shepherd
//
//  Created by Michael Cantrell on 3/4/17.
//
//

#ifndef LevelEditor_h
#define LevelEditor_h

#include "Window.h"
#include "World.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

class LevelEditor {
public:

    // static methods here:
    static void drawLevelEditor(Window &window, World *world);
    
};
#endif /* LevelEditor_h */



