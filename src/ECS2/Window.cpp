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

#include "Camera.h"
#include "RigidBody.h"

#define CAMERA_SPEED 0.2
#define CAMERA_STOPPED_THRESHOLD 0.1
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

World* Window::world = nullptr;
std::vector<WindowCallbackDelegate*> Window::windowCallbackDelegates;

Window::Window(World *w) {
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
        delegate->KeyPressed(world, width, height, key, action);
    }
    
    // Check keys
    if (action == GLFW_PRESS) {
        if (key == GLFW_KEY_ESCAPE) {
            glfwSetWindowShouldClose(window, GL_TRUE);
        }
    }
}

// static void mouse_callback(GLFWwindow *window, int button, int action, int mods) {
//    double posX, posY;
//    if (action == GLFW_PRESS) {
//       glfwGetCursorPos(window, &posX, &posY);
//       cout << "Pos X " << posX <<  " Pos Y " << posY << endl;
// 	}
// }

void Window::MouseMoveCallback(GLFWwindow *window, double posX, double posY) {
    // Get current window size.
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // Execute all registered callbacks
    for (WindowCallbackDelegate *delegate : windowCallbackDelegates) {
        delegate->MouseMoved(world, width, height, posX, posY);
    }
}

static void resize_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}

int Window::Initialize() {
    // Set error callback.
    glfwSetErrorCallback(error_callback);
    // Initialize the library.
    if(!glfwInit()) {
        return -1;
    }
    //request the highest possible version of OGL - important for mac
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    
    // Create a windowed mode window and its OpenGL context.
    window = glfwCreateWindow(960, 720, "Tyler's Awesome Window", NULL, NULL);
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
    
    // Set vsync.
    glfwSwapInterval(1);
    // Set keyboard callback.
    glfwSetKeyCallback(window, Window::KeyCallback);
    //set the mouse call back
    // glfwSetMouseButtonCallback(window, mouse_callback);
    // Set the mouse move call back
    glfwSetCursorPosCallback(window, Window::MouseMoveCallback);
    //set the window resize call back
    glfwSetFramebufferSizeCallback(window, resize_callback);
    
    return 0;
}

void Window::Terminate() {
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::Update() {
    SwapBuffers();
    PollEvents();
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

void Window::AddWindowCallbackDelegate(WindowCallbackDelegate *delegate) {
    windowCallbackDelegates.push_back(delegate);
}
