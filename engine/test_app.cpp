#include "test_app.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace engine {
    struct SimplePushConstantData {
        glm::mat3 transform;
        // align the size of the push constant to 16 bytes
        alignas(16) glm::vec3 color;
    };

    TestApp::TestApp() {
        loadGameObjects();
        createPipelineLayout();
        recreateSwapChain();
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

    void TestApp::loadGameObjects() {
        // create a model with a triangle
        std::vector<Model::Vertex> vertices = {
            {{0.0f, -0.5f, 1.0f}, {1.0f, 0.0f, 0.0f}},
            {{0.5f, 0.5f, 1.0f}, {0.0f, 1.0f, 0.0f}},
            {{-0.5f, 0.5f, 1.0f}, {0.0f, 0.0f, 1.0f}}
        };
        auto model = std::make_shared<Model>(device, vertices);

        auto triangle = GameObject::createGameObject();
        triangle.model = model;
        triangle.color = {.1f, .8f, .1f};
        triangle.transform2d.translation.x = 0.2f;
        triangle.transform2d.scale = {1.2f, 0.5f};
        triangle.transform2d.rotation = 0.25f * glm::two_pi<float>();

        gameObjects.push_back(std::move(triangle));
    }

    void TestApp::createPipelineLayout() {
        // implement the push constant
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(SimplePushConstantData);

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        // not using any shader stages
        pipelineLayoutInfo.setLayoutCount = 0; 
        pipelineLayoutInfo.pSetLayouts = nullptr; 
        pipelineLayoutInfo.pushConstantRangeCount = 1; 
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; 

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        }
    }

    void TestApp::recreateSwapChain(){
        auto extent = window.getExtent();
        while (extent.width == 0 || extent.height == 0) {
            extent = window.getExtent();
            glfwWaitEvents();
        }
        vkDeviceWaitIdle(device.device());

        if (swapChain == nullptr) {
            swapChain = std::make_unique<SwapChain> (device, extent);
        }
        else{
            swapChain = std::make_unique<SwapChain> (device, extent, std::move(swapChain));
            if (swapChain->imageCount() != commandBuffers.size()) {
                freeCommandBuffers();
                createCommandBuffers();
            }
        }

        createPipeline();
    }

    void TestApp::createPipeline(){
        assert(swapChain != nullptr && "Swap chain is null");
        assert(pipelineLayout != nullptr && "Pipeline layout is null");
        // create the pipeline with:
        // the default pipelineconfiginfo
        // the default pipeline layout,
        // the render pass from the swap chain
        // simple_shader.vert and simple_shader.frag
        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);
        pipelineConfig.renderPass = swapChain->getRenderPass();
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
        commandBuffers.resize(swapChain->imageCount());
        // allocate the command buffers
        VkCommandBufferAllocateInfo allocInfo{};
        allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
        allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
        allocInfo.commandPool = device.getCommandPool();
        allocInfo.commandBufferCount = static_cast<uint32_t>(commandBuffers.size());
        if (vkAllocateCommandBuffers(device.device(), &allocInfo, commandBuffers.data()) != VK_SUCCESS) {
            throw std::runtime_error("Failed to allocate command buffers");
        }

    }

    void TestApp::freeCommandBuffers(){
        vkFreeCommandBuffers(
            device.device(), 
            device.getCommandPool(), 
            static_cast<uint32_t>(commandBuffers.size()), 
            commandBuffers.data());
        commandBuffers.clear();
    }

    void TestApp::recordCommandBuffer(int imageIndex){
        // record the command buffers
        VkCommandBufferBeginInfo beginInfo{};
        beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;

        if (vkBeginCommandBuffer(commandBuffers[imageIndex], &beginInfo) != VK_SUCCESS) {
            throw std::runtime_error("Failed to begin recording command buffer");
        }
        // begin the render pass
        VkRenderPassBeginInfo renderPassInfo{};
        renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
        renderPassInfo.renderPass = swapChain->getRenderPass();
        renderPassInfo.framebuffer = swapChain->getFrameBuffer(imageIndex);

        renderPassInfo.renderArea.offset = {0, 0};
        renderPassInfo.renderArea.extent = swapChain->getSwapChainExtent();

        std::vector<VkClearValue> clearValues(2);
        clearValues[0].color = {0.01f, 0.01f, 0.01f, 1.0f};
        clearValues[1].depthStencil = {1.0f, 0};
        renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
        renderPassInfo.pClearValues = clearValues.data();

        // begin the render pass
        vkCmdBeginRenderPass(commandBuffers[imageIndex], &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

        // set the viewport and scissor dynamically
        VkViewport viewport{};
        viewport.x = 0.0f;
        viewport.y = 0.0f;
        viewport.width = static_cast<float>(swapChain->getSwapChainExtent().width);
        viewport.height = static_cast<float>(swapChain->getSwapChainExtent().height);
        viewport.minDepth = 0.0f;
        viewport.maxDepth = 1.0f;
        VkRect2D scissor{{0, 0}, swapChain->getSwapChainExtent()};
        vkCmdSetViewport(commandBuffers[imageIndex], 0, 1, &viewport);
        vkCmdSetScissor(commandBuffers[imageIndex], 0, 1, &scissor);

        /* 
        Note: to manage multiple pipelines' vertex buffer and other resources,
        bind the pipelines one by one
        */
        
        renderGameObjects(commandBuffers[imageIndex]);

        // end the render pass
        vkCmdEndRenderPass(commandBuffers[imageIndex]);
        if (vkEndCommandBuffer(commandBuffers[imageIndex]) != VK_SUCCESS) {
            throw std::runtime_error("Failed to record command buffer");
        }
    }

    void TestApp::renderGameObjects(VkCommandBuffer commandBuffer){
        // bind the pipeline
        pipeline->bind(commandBuffer);
        for (auto& obj : gameObjects) {
            obj.transform2d.rotation = glm::mod(obj.transform2d.rotation + 0.01f, glm::two_pi<float>());
            SimplePushConstantData push{};
            push.transform = obj.transform2d.mat3();
            push.color = obj.color;
            // push constant
            vkCmdPushConstants(
                commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(SimplePushConstantData),
                &push
            );

            obj.model->bind(commandBuffer);
            obj.model->draw(commandBuffer);
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
        VkResult result = swapChain->acquireNextImage(&imageIndex);

        if (result == VK_ERROR_OUT_OF_DATE_KHR){
            recreateSwapChain();
            return;
        }

        if (result != VK_SUCCESS && result != VK_SUBOPTIMAL_KHR) {
            throw std::runtime_error("Failed to acquire next image");
        }

        recordCommandBuffer(imageIndex);
        result = swapChain->submitCommandBuffers(&commandBuffers[imageIndex], &imageIndex);
        if (result == VK_ERROR_OUT_OF_DATE_KHR || result == VK_SUBOPTIMAL_KHR || window.wasResized()) {
            window.resetResizedFlag();
            recreateSwapChain();
            return;
        } else if (result != VK_SUCCESS) {
            throw std::runtime_error("Failed to present swap chain image");
        }
    }
}