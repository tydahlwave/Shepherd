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
#include "ImguiUpdateDelegate.h"

typedef void (*MouseCallback)(int, int, double, double); // windowWidth, windowHeight, mouseX, mouseY

struct WindowSize {
    int width, height;
};

class Window {
public:
    Window(World *w, int width, int height);
	Window() {};
    virtual ~Window();
    
    static bool drawGUI;
    static bool drawMouse;
    static bool drawWireframes;
    static bool drawAABBs;
    
    int Initialize();
    void Terminate();
    void Update();
    
    int GetHeight();
    int GetWidth();
    
    bool ShouldClose();
    void SwapBuffers();
    void PollEvents();
    void UpdateImgui();
    static void KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods);
    static void MouseMoveCallback(GLFWwindow *window, double posX, double posY);
    static void MouseButtonCallback(GLFWwindow *window, int button, int action, int mods);
	static void MouseScrollCallback(GLFWwindow *window, double dx, double dy);
    static void AddWindowCallbackDelegate(WindowCallbackDelegate *delegate);
    static void AddImguiUpdateDelegate(ImguiUpdateDelegate *delegate);
	static void DeleteCallbackDelegates();
private:
    GLFWwindow *window; // Main application window
    int _width;
    int _height;
    static World *world;
    static std::vector<WindowCallbackDelegate*> windowCallbackDelegates;
    static std::vector<ImguiUpdateDelegate*> imguiUpdateDelegates;
};

#endif /* Window_h */
