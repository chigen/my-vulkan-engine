#pragma once

/* 
    Note:
    For one application, it can contain multiple render systems,
    but one renderer

    In one render system, it contains：
    the pipeline, pipeline layout,
    some simple push constant struct, 
    and game objects

    In renderer, it contains the configuration of:
    the swap chain,
    command buffers,
    draw frame function,

*/

/* 
    Pipeline: defines the entire rendering process from input 
    vertices to final pixel output to the framebuffer

    Pipeline layout: defines the organizational structure of 
    the resources required by the shader in the pipeline, 
    including uniform buffers (Uniform Buffers), samplers 
    (Samplers), images (Images), etc. It describes how these 
    resources are accessed by shaders through Descriptor Set 
    Layouts. Pipeline layout allows you to reuse the same 
    resource organization across different pipelines, even if 
    those pipelines execute different shader code.
*/

#include "pipeline.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "camera.hpp"
#include "frame_info.hpp"

#include <memory>
#include <vector>
#include <cassert>
#include <stdexcept>

namespace engine {
    class PointLightSystem {
        public:
            PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout);
            ~PointLightSystem();
            // delete copy constructor and operator to avoid copying the renderer
            PointLightSystem(const PointLightSystem&) = delete;
            PointLightSystem& operator=(const PointLightSystem&) = delete;

            void render(FrameInfo& frameInfo);
            void update(FrameInfo& frameInfo, GlobalUbo& ubo);
            
        private:
            void createPipelineLayout(VkDescriptorSetLayout globalSetLayout);
            void createPipeline(VkRenderPass renderPass);

            glm::mat4 createRotations(int axis, float rotationSpeed);

            // device is initialized in app launcher
            Device& device;
            std::unique_ptr<Pipeline> pipeline;
            VkPipelineLayout pipelineLayout;

    };
}