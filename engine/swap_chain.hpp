#pragma once

#include "device.hpp"
#include <vulkan/vulkan.h>

#include <string>
#include <vector>
#include <iostream>
#include <memory>

namespace engine {
    /* 
        This Swap chain class is responsible for managing the swap chain.
        It contains the following methods:
        Create swap chain
        Create image views
        Create depth resources
        Create render pass
        Create frame buffers
        synchronization objects
        
        Each frame buffer contains the image and depth data
    */
    class SwapChain{
        public:
            static constexpr int MAX_FRAMES_IN_FLIGHT = 2;

            SwapChain(Device& deviceRef, VkExtent2D windowExtent);
            SwapChain(Device& deviceRef, VkExtent2D windowExtent, std::shared_ptr<SwapChain> previous);
            ~SwapChain();

            SwapChain(const SwapChain&) = delete;
            SwapChain& operator=(const SwapChain&) = delete;

            VkFramebuffer getFrameBuffer(int index) { return swapChainFramebuffers[index]; }
            VkRenderPass getRenderPass() { return renderPass; }
            VkImageView getImageView(int index) { return swapChainImageViews[index]; }
            size_t imageCount() { return swapChainImages.size(); }
            VkFormat getSwapChainImageFormat() { return swapChainImageFormat; }
            VkExtent2D getSwapChainExtent() { return swapChainExtent; }
            uint32_t width() { return swapChainExtent.width; }
            uint32_t height() { return swapChainExtent.height; }

            float extentAspectRatio() {
                return static_cast<float>(swapChainExtent.width) / static_cast<float>(swapChainExtent.height);
            }
            VkFormat findDepthFormat();

            VkResult acquireNextImage(uint32_t *imageIndex);
            VkResult submitCommandBuffers(const VkCommandBuffer *buffers, uint32_t *imageIndex);

            bool compareSwapFormats(const SwapChain& swapChain) const{
                return (swapChain.swapChainImageFormat == swapChainImageFormat
                    && swapChain.swapChainDepthFormat == swapChainDepthFormat);
            }

        private:
            void init(); 
            void createSwapChain();
            void createImageViews();
            void createDepthResources();
            void createRenderPass();
            void createFramebuffers();
            void createSyncObjects();

            // helper functions:
            // choose the surface format for the swap chain
            VkSurfaceFormatKHR chooseSurfaceFormat(
                const std::vector<VkSurfaceFormatKHR>& availableFormats);
            // choose the presentation mode for the swap chain
            VkPresentModeKHR choosePresentMode(
                const std::vector<VkPresentModeKHR>& availablePresentModes);
            // choose the swap extent
            VkExtent2D chooseSwapExtent(const VkSurfaceCapabilitiesKHR& capabilities);

            Device& device;
            VkExtent2D windowExtent;
            VkExtent2D swapChainExtent;

            VkSwapchainKHR swapChain;
            std::shared_ptr<SwapChain> oldSwapChain;

            std::vector<VkImage> swapChainImages;
            std::vector<VkImageView> swapChainImageViews;
            std::vector<VkImage> depthImages;
            std::vector<VkDeviceMemory> depthImageMemorys;
            std::vector<VkImageView> depthImageViews;

            VkFormat swapChainImageFormat;
            VkFormat swapChainDepthFormat;

            std::vector<VkFramebuffer> swapChainFramebuffers;
            VkRenderPass renderPass;

            std::vector<VkSemaphore> imageAvailableSemaphores;
            std::vector<VkSemaphore> renderFinishedSemaphores;
            std::vector<VkFence> inFlightFences;
            std::vector<VkFence> imagesInFlight;
            size_t currentFrame = 0;
    };
}  // namespace engine