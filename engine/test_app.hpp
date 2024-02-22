#pragma once

/* 
    This app is to test window function and pipeline function
    It will create a default window and create a pipeline with 
    simple_shader.vert and simple_shader.frag
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

#include "window.hpp"
#include "pipeline.hpp"
#include "device.hpp"
#include "swap_chain.hpp"
#include "model.hpp"

#include <memory>
#include <vector>

namespace engine {
    class TestApp {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            TestApp();
            ~TestApp();

            void run();

        private:
            void loadModel();
            void createPipelineLayout();
            void createPipeline();
            void createCommandBuffers();
            void drawFrame();

            Window window{WIDTH, HEIGHT, "Test App"};
            Device device{window};
            SwapChain swapChain{device, window.getExtent()};
            std::unique_ptr<Pipeline> pipeline;
            VkPipelineLayout pipelineLayout;
            std::vector<VkCommandBuffer> commandBuffers;
            std::unique_ptr<Model> model;
    };
}