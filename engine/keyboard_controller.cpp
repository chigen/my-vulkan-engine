#include "keyboard_controller.hpp"

namespace engine{
    void KeyboardController::moveInXZPlane(GLFWwindow* window, float dt, GameObject &gameObject){
        glm::vec3 rotation{0.f};
        if(glfwGetKey(window, keys.lookLeft)==GLFW_PRESS) rotation.y += 1.f;       
        if(glfwGetKey(window, keys.lookRight)==GLFW_PRESS) rotation.y -= 1.f;
        if(glfwGetKey(window, keys.lookUp)==GLFW_PRESS) rotation.x += 1.f;
        if(glfwGetKey(window, keys.lookDown)==GLFW_PRESS) rotation.x -= 1.f;

        // check if the rotation vector is not zero
        if(glm::dot(rotation, rotation) > std::numeric_limits<float>::epsilon()){
            gameObject.transform3d.rotation += glm::normalize(rotation) * rotationSpeed * dt;
        }
        // limit pitch values between about +/- 85ish degrees
        gameObject.transform3d.rotation.x = glm::clamp(gameObject.transform3d.rotation.x, -1.5f, 1.5f);
        gameObject.transform3d.rotation.y = glm::mod(gameObject.transform3d.rotation.y, glm::two_pi<float>());

        float yaw = gameObject.transform3d.rotation.y;
        const glm::vec3 forwardDir{sin(yaw), 0.f, cos(yaw)};
        const glm::vec3 rightDir{forwardDir.z, 0.f, -forwardDir.x};
        const glm::vec3 upDir{0.f, -1.f, 0.f};

        glm::vec3 moveDir{0.f};
        if (glfwGetKey(window, keys.moveForward) == GLFW_PRESS) moveDir += forwardDir;
        if (glfwGetKey(window, keys.moveBackward) == GLFW_PRESS) moveDir -= forwardDir;
        if (glfwGetKey(window, keys.moveRight) == GLFW_PRESS) moveDir += rightDir;
        if (glfwGetKey(window, keys.moveLeft) == GLFW_PRESS) moveDir -= rightDir;
        if (glfwGetKey(window, keys.moveUp) == GLFW_PRESS) moveDir += upDir;
        if (glfwGetKey(window, keys.moveDown) == GLFW_PRESS) moveDir -= upDir;

        if (glm::dot(moveDir, moveDir) > std::numeric_limits<float>::epsilon()) {
            gameObject.transform3d.translation += movementSpeed * dt * glm::normalize(moveDir);
        }
    }
}