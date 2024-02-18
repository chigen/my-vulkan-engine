#include "window.hpp"

namespace engine {
    void Window::initWindow() {
        // initialize the library
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        // set the window hints
        // disable OpenGL and make the window not resizable
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
    }

    Window::~Window() {
        // destroy the window
        glfwDestroyWindow(window);

        // terminate the library
        glfwTerminate();
    }

    void Window::createWindowSurface(VkInstance instance, VkSurfaceKHR* surface) {
        if (glfwCreateWindowSurface(instance, window, nullptr, surface) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create window surface");
        }
    }
}