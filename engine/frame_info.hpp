#pragma once

#include "camera.hpp"

#include <vulkan/vulkan.h>

namespace engine {
    struct FrameInfo {
        int frameIndex;
        float frameTime;
        VkCommandBuffer commandBuffer;
        Camera& camera;
    };
}