#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <memory>

#include "OpenGL/OGLRenderer.hpp"

// Callbacks
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods);

// Global renderer instance
OGLRenderer* oglRenderer = nullptr;

int main() {
    // Initialize GLFW
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Create window
    GLFWwindow* window = glfwCreateWindow(1920, 1080, "Model Viewer", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    
    // Set up callbacks
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    //glfwSetKeyCallback(window, key_callback);
    glfwSetMouseButtonCallback(window, mouse_button_callback);

    // Initialize renderer
    oglRenderer = new OGLRenderer(window);
    if (!oglRenderer->init(1920, 1080)) {
        glfwTerminate();
        return -1;
    }



    // Main loop
    while (!glfwWindowShouldClose(window)) {
        // Calculate delta time
        static float lastFrame = 0.0f;
        float currentFrame = static_cast<float>(glfwGetTime());
        float deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Render frame
        oglRenderer->draw(deltaTime);

        // Swap buffers and poll events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    delete oglRenderer;
    glfwTerminate();
    return 0;
}

// Input handling callbacks ------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    if (oglRenderer) oglRenderer->setSize(width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos) {
    if (oglRenderer) oglRenderer->handleMousePositionEvents(xpos, ypos);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yoffset) {
    // Implement if needed in OGLRenderer
}

/*
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (oglRenderer) oglRenderer->handleKeyEvents(key, scancode, action, mods);
    
    // Toggle mouse lock with Left Control
    if (key == GLFW_KEY_LEFT_CONTROL && action == GLFW_PRESS) {
        bool currentLock = oglRenderer->getMouseLock();
        oglRenderer->setMouseLock(!currentLock);
        glfwSetInputMode(window, GLFW_CURSOR, 
            currentLock ? GLFW_CURSOR_NORMAL : GLFW_CURSOR_DISABLED);
    }
}
*/
void mouse_button_callback(GLFWwindow* window, int button, int action, int mods) {
    if (oglRenderer) oglRenderer->handleMouseButtonEvents(button, action, mods);
}