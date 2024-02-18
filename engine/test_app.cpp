#include "test_app.hpp"

namespace engine {
    void TestApp::run() {
        while (!window.shouldClose()) {
            glfwPollEvents();
        }
    }
}