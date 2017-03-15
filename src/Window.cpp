//
//  Window.cpp
//  SimpleGame
//
//  Created by Tyler Dahl on 1/22/17.
//
//

#include "Window.h"

#include <iostream>
#include <stdio.h>
#include <cmath>

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "Components/Camera.h"
#include "Components/RigidBody.h"

#define CAMERA_SPEED 0.2
#define CAMERA_STOPPED_THRESHOLD 0.1
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

World* Window::world = nullptr;
bool Window::drawGUI = false;
bool Window::drawMouse = false;
bool Window::drawWireframes = false;
bool Window::drawAABBs = false;
std::vector<WindowCallbackDelegate*> Window::windowCallbackDelegates;
std::vector<ImguiUpdateDelegate*> Window::imguiUpdateDelegates;

Window::Window(World *w, int width, int height) :_width(width), _height(height) {
    Window::world = w;
    Initialize();
}

Window::~Window() {
    Terminate();
}

static void error_callback(int error, const char *description) {
    std::cerr << description << std::endl;
}

void Window::KeyCallback(GLFWwindow *window, int key, int scancode, int action, int mods) {
    // Get current window size.
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // Execute all registered callbacks
    for (WindowCallbackDelegate *delegate : windowCallbackDelegates) {

		if (((glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) && (delegate->type == 0)) ||
			((glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) && (delegate->type == 1)) ||
			(delegate->type == 2))
        delegate->KeyPressed(world, width, height, key, action);
    }
    
    // Check keys
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        } else if (key == GLFW_KEY_G) {
            drawGUI = !drawGUI;
        } else if (key == GLFW_KEY_M) {
            drawMouse = !drawMouse;
            // Disable cursor (allows unlimited scrolling)
            glfwSetInputMode(window, GLFW_CURSOR, drawMouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
            // If cursor is becoming disabled, move it to the center of the screen
            if (!drawMouse) {
                glfwSetCursorPos(window, width/2, height/2);
            }
        } else if (key == GLFW_KEY_L) {
            drawWireframes = !drawWireframes;
            glPolygonMode(GL_FRONT_AND_BACK, drawWireframes ? GL_LINE : GL_FILL);
        } else if (key == GLFW_KEY_V) {
            drawAABBs = !drawAABBs;
        }
    }
}

void Window::checkMouse() {
	glfwSetInputMode(window, GLFW_CURSOR, drawMouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
}

void Window::MouseMoveCallback(GLFWwindow *window, double posX, double posY) {
    // Get current window size.
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // Execute all registered callbacks
    for (WindowCallbackDelegate *delegate : windowCallbackDelegates) {
		if (((glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) && (delegate->type == 0)) ||
			((glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) && (delegate->type == 1)) ||
			(delegate->type == 2))
			delegate->MouseMoved(world, width, height, posX, posY);
    }
}

void Window::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
	double xPos, yPos;
	glfwGetCursorPos(window, &xPos, &yPos);

	// Execute all registered callbacks
	for (WindowCallbackDelegate *delegate : windowCallbackDelegates) {
		if (((glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) && (delegate->type == 0)) ||
			((glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) && (delegate->type == 1)) ||
			(delegate->type == 2))
			delegate->MouseClicked(world, xPos, yPos, button, action);
    }
}

void Window::MouseScrollCallback(GLFWwindow *window, double dx, double dy) {
	// Execute all registered callbacks
	for (WindowCallbackDelegate *delegate : windowCallbackDelegates) {
		if (((glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_NORMAL) && (delegate->type == 0)) ||
			((glfwGetInputMode(window, GLFW_CURSOR) == GLFW_CURSOR_DISABLED) && (delegate->type == 1)) ||
			(delegate->type == 2))
			delegate->MouseScrolled(world, dx, dy);
	}
}

static void resize_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


void Window::DeleteCallbackDelegates() {
	windowCallbackDelegates.clear();
    imguiUpdateDelegates.clear();
}
int Window::Initialize() {
    // Set error callback.
    glfwSetErrorCallback(error_callback);
    // Initialize the library.
    if(!glfwInit()) {
        return -1;
    }
#ifndef WIN32
    //request the highest possible version of OGL - important for mac
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
#endif

    // Create a windowed mode window and its OpenGL context.
    window = glfwCreateWindow(_width, _height, "Tyler's Awesome Window", NULL, NULL);
    if(!window) {
        glfwTerminate();
        return -1;
    }
    // Make the window's context current.
    glfwMakeContextCurrent(window);
    // Initialize GLEW.
    glewExperimental = true;
    if(glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }
    //weird bootstrap of glGetError
    glGetError();
    std::cout << "OpenGL version: " << glGetString(GL_VERSION) << std::endl;
    std::cout << "GLSL version: " << glGetString(GL_SHADING_LANGUAGE_VERSION) << std::endl;
    
    // Disable cursor (allows unlimited scrolling)
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    
    // Setup ImGui binding
    ImGui_ImplGlfwGL3_Init(window, false);
    
    // Set vsync.
    glfwSwapInterval(1);
    // Set keyboard callback.
    glfwSetKeyCallback(window, Window::KeyCallback);
    //set the mouse call back
    glfwSetMouseButtonCallback(window, Window::MouseButtonCallback);
    // Set the mouse move call back
    glfwSetCursorPosCallback(window, Window::MouseMoveCallback);
	// set the mouse scroll call back
	glfwSetScrollCallback(window, Window::MouseScrollCallback);
    //set the window resize call back
    glfwSetFramebufferSizeCallback(window, resize_callback);
    
    // Set cursor to center of screen if disabled
    if (!drawMouse) glfwSetCursorPos(window, _width/2, _height/2);
    
    return 0;
}

void Window::Terminate() {
    ImGui_ImplGlfwGL3_Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::Update() {
    PollEvents();
    //if (drawGUI) UpdateImgui();
    UpdateImgui();
    
    SwapBuffers();
}

void Window::UpdateImgui() {
    // Prevent gui from being drawn in wireframe mode
    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    
    // Begin next gui frame
    ImGui_ImplGlfwGL3_NewFrame();
    
    // Execute all registered callbacks
    for (ImguiUpdateDelegate *delegate : imguiUpdateDelegates) {
        delegate->ImguiUpdate(world, drawGUI);
    }
    
    // Render the gui windows
    ImGui::Render();
    
    // Revert wireframe mode to how it was
    glPolygonMode(GL_FRONT_AND_BACK, drawWireframes ? GL_LINE : GL_FILL);
}

int Window::GetHeight() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return height;
}

int Window::GetWidth() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    return width;
}

bool Window::ShouldClose() {
    return glfwWindowShouldClose(window);
}

void Window::SwapBuffers() {
    // Swap front and back buffers.
    glfwSwapBuffers(window);
}

void Window::PollEvents() {
    // Poll for and process events.
    glfwPollEvents();
}


/*
 * type : 0 - only when mouse is drawn
 *		: 1 - only when mouse is not drawn
 *		: 2 - both
*/
void Window::AddWindowCallbackDelegate(WindowCallbackDelegate *delegate, int type) {
	delegate->type = type;
    windowCallbackDelegates.push_back(delegate);
}

void Window::AddImguiUpdateDelegate(ImguiUpdateDelegate *delegate) {
    imguiUpdateDelegates.push_back(delegate);
}
