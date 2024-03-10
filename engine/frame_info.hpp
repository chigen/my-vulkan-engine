#pragma once

#include "camera.hpp"
#include "game_object.hpp"

#include <vulkan/vulkan.h>

/* 
    put the configuration if frame info here
 */

namespace engine {
    #define MAX_POINT_LIGHTS 10
    struct PointLight {
        glm::vec4 position{};
        glm::vec4 color{};
    };

    struct GlobalUbo {
        glm::mat4 project{1.f};
        glm::mat4 view{1.f};
        glm::mat4 inverseView{1.f};
        // this lightDirection is for parrallel light
        // glm::vec3 lightDirection = glm::normalize(glm::vec3(1.f, -3.f, -1.f));

        // for point light:
        // ambient light, w is its intensity
        glm::vec4 ambientLight{1.f, 1.f, 1.f, 0.02f};
        PointLight pointLights[MAX_POINT_LIGHTS];
        int numLights;
    };

    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera& camera;
        VkDescriptorSet globalDescriptorSet;
        GameObject::Map &gameObjects; 
    };
}