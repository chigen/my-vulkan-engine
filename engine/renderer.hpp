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

#include "window.hpp"
#include "device.hpp"
#include "swap_chain.hpp"

#include <memory>
#include <vector>
#include <cassert>
#include <stdexcept>

namespace engine {
    class Renderer {
        public:

            Renderer(Device &device, Window &window);
            ~Renderer();

            // delete copy constructor and operator to avoid copying the renderer
            Renderer(const Renderer&) = delete;
            Renderer& operator=(const Renderer&) = delete;

            VkRenderPass getSwapChainRenderPass() const { return swapChain->getRenderPass(); }
            bool isFrameInProgress() const { return isFrameStarted; }

            VkCommandBuffer getCurrentCommandBuffer() const {
                assert(isFrameStarted && "Cannot get command buffer when frame not in progress");
                return commandBuffers[currentFrameIndex];
            }

            int getFrameIndex() const {
                assert(isFrameStarted && "Cannot get frame index when frame not in progress");
                return currentFrameIndex;
            }

            VkCommandBuffer beginFrame();
            void endFrame();
            void beginSwapChainRenderPass(VkCommandBuffer commandBuffer);
            void endSwapChainRenderPass(VkCommandBuffer commandBuffer);

        private:
            void createCommandBuffers();
            void freeCommandBuffers();
            void recreateSwapChain();

            // device and window are initialized in app launcher
            Window &window;
            Device &device;
            std::unique_ptr<SwapChain> swapChain;
            std::vector<VkCommandBuffer> commandBuffers;

            // seperate frame index and image index
            int currentFrameIndex{0};
            uint32_t currentImageIndex{0};
            bool isFrameStarted{false};
    };
}