#pragma once

/* 
    This file contains the pipeline class, which is responsible for managing the rendering pipeline.
    It contains the following methods:
    Read shader files
    Create pipeline
 */

#include <string>
#include <vector>
#include <fstream>
#include <iostream>

#include "device.hpp"

namespace engine
{
    struct PipelineConfigInfo{
        PipelineConfigInfo() = default;

        // avoid copying the pipeline config info
        PipelineConfigInfo(const PipelineConfigInfo&) = delete;
        PipelineConfigInfo& operator=(const PipelineConfigInfo&) = delete;

        VkViewport viewport;
        VkRect2D scissor;
        // VkPipelineViewportStateCreateInfo viewportInfo;
        VkPipelineInputAssemblyStateCreateInfo inputAssemblyInfo;
        VkPipelineRasterizationStateCreateInfo rasterizationInfo;
        VkPipelineMultisampleStateCreateInfo multisampleInfo;
        VkPipelineColorBlendAttachmentState colorBlendAttachment;
        VkPipelineColorBlendStateCreateInfo colorBlendInfo;
        VkPipelineDepthStencilStateCreateInfo depthStencilInfo;
        // Pipeline layout will be created in app launcher separately
        VkPipelineLayout pipelineLayout = nullptr;
        VkRenderPass renderPass = nullptr;
        uint32_t subpass = 0;
    };

    class Pipeline{
        public:
            Pipeline(
                Device& device, 
                const std::string& vertFile, 
                const std::string& fragFile, 
                const PipelineConfigInfo& configInfo);
            ~Pipeline();

            // delete copy constructor and operator to avoid copying the pipeline
            Pipeline(const Pipeline&) = delete;
            void operator=(const Pipeline&) = delete;

            void bind(VkCommandBuffer commandBuffer);

            static void defaultPipelineConfigInfo(
                PipelineConfigInfo& configinfo, uint32_t width, uint32_t height);

        private:
            static std::vector<char> readFile (const std::string& filename);
            // create a graphics pipeline with the pipelineConfigInfo, vertex shader and fragment shader
            void createGraphicsPipeline(const std::string& vertFile, 
                    const std::string& fragFile, const PipelineConfigInfo& configInfo);
            void createShaderModule(const std::vector<char>& code, VkShaderModule* VkShaderModule);

            Device& device;
            VkPipeline graphicsPipeline;
            VkShaderModule vertShaderModule;
            VkShaderModule fragShaderModule;
    };
} 
