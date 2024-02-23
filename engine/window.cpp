#include "window.hpp"

namespace engine {
    Window::Window(int w, int h, const std::string& name)
        : width(w), height(h), windowName(name){
        initWindow();
    }

    void Window::initWindow() {
        // initialize the library
        if (!glfwInit()) {
            throw std::runtime_error("Failed to initialize GLFW");
        }

        // set the window hints
        // disable OpenGL and make the window not resizable
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);
        // To resize the window, change GLFW_FALSE to GLFW_TRUE
        // And more importantly, need to recreate the swap chain when the window is resized

        window = glfwCreateWindow(width, height, windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(window, this);
        glfwSetFramebufferSizeCallback(window, framebufferResizeCallback);

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

    void Window::framebufferResizeCallback(GLFWwindow* window, int width, int height){
        auto newWindow = reinterpret_cast<Window*>(glfwGetWindowUserPointer(window));
        newWindow->frameBufferResized = true;
        newWindow->width = width;
        newWindow->height = height;
    }
}