//
//  Window.h
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#ifndef Window_h
#define Window_h

#define GLEW_STATIC
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <vector>

#include "glm/glm.hpp"
#include "World.h"
#include "WindowCallbackDelegate.h"

typedef void (*MouseCallback)(int, int, double, double); // windowWidth, windowHeight, mouseX, mouseY

struct WindowSize {
    int width, height;
};

class Window {
public:
    Window(World *w);
    virtual ~Window();
    
    int Initialize();
    void Terminate();
    void Update();
    
    int GetHeight();
    int GetWidth();
    
    bool ShouldClose();
    void SwapBuffers();
    void PollEvents();
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void MouseMoveCallback(GLFWwindow *window, double posX, double posY);
    static void AddWindowCallbackDelegate(WindowCallbackDelegate *delegate);
private:
    GLFWwindow *window; // Main application window
    static World *world;
    static std::vector<WindowCallbackDelegate*> windowCallbackDelegates;
};

#endif /* Window_h */
