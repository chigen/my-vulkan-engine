#include "test_app.hpp"

namespace engine {
    TestApp::TestApp() {
        createPipelineLayout();
        createPipeline();
        createCommandBuffers();
    }

    TestApp::~TestApp() {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
        // commandBuffers is destroyed when the command pool is destroyed
    }

    void TestApp::run() {
        while (!window.shouldClose()) {
            glfwPollEvents();
            drawFrame();
        }
        // wait for the device (gpu) to finish before cleaning up
        vkDeviceWaitIdle(device.device());
    }

    void TestApp::createPipelineLayout() {
        // create a default pipeline layout
        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        // not using any shader stages
        pipelineLayoutInfo.setLayoutCount = 0; 
        pipelineLayoutInfo.pSetLayouts = nullptr; 
        pipelineLayoutInfo.pushConstantRangeCount = 0; 
        pipelineLayoutInfo.pPushConstantRanges = nullptr; 

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        }
    }

    void TestApp::createPipeline(){
        // create the pipeline with:
        // the default pipelineconfiginfo
        // the default pipeline layout,
        // the render pass from the swap chain
        // simple_shader.vert and simple_shader.frag
        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(
            pipelineConfig, swapChain.width(), swapChain.height());
        pipelineConfig.renderPass = swapChain.getRenderPass();
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
            device, 
            "shader/simple_shader.vert.spv", 
            "shader/simple_shader.frag.spv", 
            pipelineConfig);
    }

    void TestApp::createCommandBuffers(){
        // procedure to create command buffers:
        // 1. Allocate command buffers
        // 2. Record command buffers
        // bind the pipeline

        // resize to the corresponding swap chain image count, double or triple buffering
        commandBuffers.resize(swapChain.imageCount());
        // allocate the command buffers
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.commandBufferCount = (uint32_t) commandBuffers.size();
        if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers");
        }

        // record the command buffers
        for(int i=0; i<commandBuffers.size(); ++i){
            VkCommandBufferBeginInfo beginInfo{};
            beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

            if (vkBeginCommandBuffer(commandBuffers[i], &beginInfo) != VK_SUCCESS) {
                throw std::runtime_error("Failed to begin recording command buffer");
            }
            // begin the render pass
            VkRenderPassBeginInfo renderPassInfo{};
            renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
            renderPassInfo.renderPass = swapChain.getRenderPass();
            renderPassInfo.framebuffer = swapChain.getFrameBuffer(i);

            renderPassInfo.renderArea.offset = {0, 0};
            renderPassInfo.renderArea.extent = swapChain.getSwapChainExtent();

            std::vector<VkClearValue> clearValues(2);
            clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
            clearValues[1].depthStencil = {1.0f, 0};
            renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
            renderPassInfo.pClearValues = clearValues.data();

            // begin the render pass
            vkCmdBeginRenderPass(commandBuffers[i], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
            // bind the pipeline
            pipeline->bind(commandBuffers[i]);
            vkCmdDraw(commandBuffers[i], 3, 1, 0, 0);
            // end the render pass
            vkCmdEndRenderPass(commandBuffers[i]);
            if (vkEndCommandBuffer(commandBuffers[i]) != VK_SUCCESS) {
                throw std::runtime_error("Failed to record command buffer");
            }
        }
    }

    void TestApp::drawFrame(){
        // draw frame procedure:
        // 1. Acquire an image from the swap chain, calling vkAcquireNextImageKHR
        // (in SwapChain::acquireNextImage)
        // 2. submit the command buffer to the graphics queue, calling vkQueueSubmit
        // (in SwapChain::submitCommandBuffers)
        // 3. return the image to the swap chain for presentation, calling vkQueuePresentKHR
        // (in SwapChain::submitCommandBuffers)
        // also update the current frame in flight

        uint32_t imageIndex;
        VkResult result = swapChain.acquireNextImage(&imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to acquire next image");
        }
        result = swapChain.submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to submit command buffer");
        }
    }
}