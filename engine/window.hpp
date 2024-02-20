#pragma once

// makes the GLFW header include the Vulkan vulkan/vulkan.h
#define GLFW_INCLUDE_VULKAN
#include <GLFW/glfw3.h>

#include <string>

namespace engine {
    class Window {
        public:
            Window(int w, int h, const std::string& name) 
                : width(w), height(h), windowName(name) { initWindow(); }
            ~Window();

            // delete copy constructor and operator to avoid copying the window
            Window(const Window&) = delete;
            Window &operator=(const Window&) = delete;

            bool shouldClose() const {
                // check the window close flag
                return glfwWindowShouldClose(window);
            }

            VkExtent2D getExtent() const {
                return {static_cast<uint32_t>(width), static_cast<uint32_t>(height)};
            }

            void createWindowSurface(VkInstance instance, VkSurfaceKHR* surface);

        private:
            void initWindow();

            GLFWwindow* window;
            std::string windowName;
            const int width;
            const int height;
    };
}