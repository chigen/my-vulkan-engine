#pragma once

#include "window.hpp"
#include "game_object.hpp"

namespace engine{
    class KeyboardController{
        public:
            struct KeyMappings{
                int moveForward = GLFW_KEY_W;
                int moveBackward = GLFW_KEY_S;
                int moveLeft = GLFW_KEY_A;
                int moveRight = GLFW_KEY_D;
                int moveUp = GLFW_KEY_Q;
                int moveDown = GLFW_KEY_E;
                int lookLeft = GLFW_KEY_LEFT;
                int lookRight = GLFW_KEY_RIGHT;
                int lookUp = GLFW_KEY_UP;
                int lookDown = GLFW_KEY_DOWN;
            };

            void moveInXZPlane(GLFWwindow* window, float dt, GameObject &gameObject);

            KeyMappings keys{};
            float movementSpeed{3.0f};
            float rotationSpeed{1.5f};
    };
}