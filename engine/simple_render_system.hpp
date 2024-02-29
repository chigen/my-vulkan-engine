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
    class SimpleRenderSystem {
        public:
            SimpleRenderSystem(Device &device, VkRenderPass renderPass);
            ~SimpleRenderSystem();
            // delete copy constructor and operator to avoid copying the renderer
            SimpleRenderSystem(const SimpleRenderSystem&) = delete;
            SimpleRenderSystem& operator=(const SimpleRenderSystem&) = delete;

            void renderGameObjects(FrameInfo& frameInfo, 
                std::vector<GameObject>& gameObjects);
        private:
            void createPipelineLayout();
            void createPipeline(VkRenderPass renderPass);

            // device is initialized in app launcher
            Device& device;
            std::unique_ptr<Pipeline> pipeline;
            VkPipelineLayout pipelineLayout;
    };
}