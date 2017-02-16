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
#include "TextureLoader.h"

#define CAMERA_SPEED 0.2
#define CAMERA_STOPPED_THRESHOLD 0.1
#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

World* Window::world = nullptr;
bool Window::drawGUI = true;
bool Window::drawMouse = false;
bool Window::drawWireframes = false;
bool Window::drawAABBs = false;
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
        } else if (key == GLFW_KEY_G) {
            drawGUI = !drawGUI;
        } else if (key == GLFW_KEY_M) {
            drawMouse = !drawMouse;
            // Disable cursor (allows unlimited scrolling)
            glfwSetInputMode(window, GLFW_CURSOR, drawMouse ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
        } else if (key == GLFW_KEY_L) {
            drawWireframes = !drawWireframes;
            glPolygonMode(GL_FRONT_AND_BACK, drawWireframes ? GL_LINE : GL_FILL);
        } else if (key == GLFW_KEY_V) {
            drawAABBs = !drawAABBs;
        }
    }
}

void Window::MouseMoveCallback(GLFWwindow *window, double posX, double posY) {
    // Get current window size.
    int width, height;
    glfwGetWindowSize(window, &width, &height);
    
    // Execute all registered callbacks
    for (WindowCallbackDelegate *delegate : windowCallbackDelegates) {
        delegate->MouseMoved(world, width, height, posX, posY);
    }
}

void Window::MouseButtonCallback(GLFWwindow *window, int button, int action, int mods) {
    double xPos, yPos;
    glfwGetCursorPos(window, &xPos, &yPos);
    
    // Execute all registered callbacks
    for (WindowCallbackDelegate *delegate : windowCallbackDelegates) {
        delegate->MouseClicked(world, xPos, yPos, button, action);
    }
}

void Window::MouseScrollCallback(GLFWwindow *window, double dx, double dy) {
	// Execute all registered callbacks
	for (WindowCallbackDelegate *delegate : windowCallbackDelegates) {
		delegate->MouseScrolled(world, dx, dy);
	}
}

static void resize_callback(GLFWwindow *window, int width, int height) {
    glViewport(0, 0, width, height);
}


void Window::DeleteCallbackDelegates() {
	windowCallbackDelegates.clear();
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
    window = glfwCreateWindow(1080, 920, "Tyler's Awesome Window", NULL, NULL);
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
    
    return 0;
}

void Window::Terminate() {
    ImGui_ImplGlfwGL3_Shutdown();
    glfwDestroyWindow(window);
    glfwTerminate();
}

void Window::Update() {
    PollEvents();
    
//    bool show_test_window = true;
//    bool show_another_window = true;
//    ImVec4 clear_color = ImColor(114, 144, 154);
    
//    ImGui_ImplGlfwGL3_NewFrame();
//    
//    // 1. Show a simple window
//    // Tip: if we don't call ImGui::Begin()/ImGui::End() the widgets appears in a window automatically called "Debug"
//    {
//        static float f = 0.0f;
//        ImGui::Text("Hello, world!");
//        ImGui::SliderFloat("float", &f, 0.0f, 1.0f);
//        ImGui::ColorEdit3("clear color", (float*)&clear_color);
//        if (ImGui::Button("Test Window")) show_test_window ^= 1;
//        if (ImGui::Button("Another Window")) show_another_window ^= 1;
//        ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
//    }
//    
//    // 2. Show another simple window, this time using an explicit Begin/End pair
//    if (show_another_window)
//    {
//        ImGui::SetNextWindowSize(ImVec2(200,100), ImGuiSetCond_FirstUseEver);
//        ImGui::Begin("Another Window", &show_another_window);
//        ImGui::Text("Hello");
//        ImGui::End();
//    }
//    
//    // 3. Show the ImGui test window. Most of the sample code is in ImGui::ShowTestWindow()
//    if (show_test_window)
//    {
//        ImGui::SetNextWindowPos(ImVec2(650, 20), ImGuiSetCond_FirstUseEver);
//        ImGui::ShowTestWindow(&show_test_window);
//    }
//    
//    {
//        ImGui::SetNextWindowPos(ImVec2(300, 20), ImGuiSetCond_FirstUseEver);
//        ImGui::Begin("Terrain Settings");
//        ImGui::Text("Testing");
//        ImVec2 uv0 = ImVec2(0, 0);
//        ImVec2 uv1 = ImVec2(1, 1);
//        ImGui::Image((void*)textureTest->getTextureId(), ImVec2(128,128), uv0, uv1, ImColor(255,255,255,255), ImColor(255,255,255,128));
//        ImGui::End();
//    }
//    
//    ImGui::Render();
    
    SwapBuffers();
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
