#include "point_light_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

namespace engine {
    struct PointLightPushConstantData {
        // configuarate the push constant data for point light
        glm::vec4 position{};
        glm::vec4 color{};
        float radius;
    };

    PointLightSystem::PointLightSystem(Device &device, VkRenderPass renderPass, VkDescriptorSetLayout globalSetLayout)
        :device(device) {
        createPipelineLayout(globalSetLayout);
        createPipeline(renderPass);
    }

    PointLightSystem::~PointLightSystem() {
        vkDestroyPipelineLayout(device.device(), pipelineLayout, nullptr);
    }

    void PointLightSystem::createPipelineLayout(VkDescriptorSetLayout globalSetLayout) {
        VkPushConstantRange pushConstantRange{};
        pushConstantRange.stageFlags = VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT;
        pushConstantRange.offset = 0;
        pushConstantRange.size = sizeof(PointLightPushConstantData);

        std::vector<VkDescriptorSetLayout> descriptorSetLayouts = {globalSetLayout};

        VkPipelineLayoutCreateInfo pipelineLayoutInfo{};
        pipelineLayoutInfo.sType = VK_STRUCTURE_TYPE_PIPELINE_LAYOUT_CREATE_INFO;
        // not using any shader stages
        pipelineLayoutInfo.setLayoutCount = static_cast<uint32_t>(descriptorSetLayouts.size()); 
        pipelineLayoutInfo.pSetLayouts = descriptorSetLayouts.data(); 
        pipelineLayoutInfo.pushConstantRangeCount = 1; 
        pipelineLayoutInfo.pPushConstantRanges = &pushConstantRange; 

        if (vkCreatePipelineLayout(device.device(), &pipelineLayoutInfo, nullptr, &pipelineLayout) != VK_SUCCESS) {
            throw std::runtime_error("Failed to create pipeline layout");
        }
    }

    void PointLightSystem::createPipeline(VkRenderPass renderPass){
        assert(pipelineLayout != nullptr && "Pipeline layout is null");
        // create the pipeline with:
        // the default pipelineconfiginfo
        // the default pipeline layout,
        // the render pass from the swap chain
        // simple_shader.vert and simple_shader.frag
        PipelineConfigInfo pipelineConfig{};
        Pipeline::defaultPipelineConfigInfo(pipelineConfig);

        // clear point light system's attribute descriptions and binding descriptions as it currently does not have vertex input
        pipelineConfig.attributeDescriptions.clear();
        pipelineConfig.bindingDescriptions.clear();
        pipelineConfig.renderPass = renderPass;
        pipelineConfig.pipelineLayout = pipelineLayout;
        pipeline = std::make_unique<Pipeline>(
            device, 
            "shader/point_light.vert.spv", 
            "shader/point_light.frag.spv", 
            pipelineConfig);
    }

    void PointLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo) {
        // periodically update the point light's properties
        auto rotateLight = glm::rotate(glm::mat4(1.f), 0.5f * frameInfo.frameTime, {0.f, -1.f, 0.f});
        int lightIndex = 0;
        for (auto& kv : frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.pointLight == nullptr) continue;

            assert(lightIndex < MAX_POINT_LIGHTS && "Point lights exceed maximum specified");

            // update light position
            obj.transform3d.translation = glm::vec3(rotateLight * glm::vec4(obj.transform3d.translation, 1.f));

            // copy light to ubo
            ubo.pointLights[lightIndex].position = glm::vec4(obj.transform3d.translation, 1.f);
            ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->intensity);

            lightIndex += 1;
        }
        ubo.numLights = lightIndex;
    }

    void PointLightSystem::render(FrameInfo& frameInfo){
        // bind the pipeline
        pipeline->bind(frameInfo.commandBuffer);

        vkCmdBindDescriptorSets(
            frameInfo.commandBuffer,
            VK_PIPELINE_BIND_POINT_GRAPHICS,
            pipelineLayout,
            0,
            1,
            &frameInfo.globalDescriptorSet,
            0,
            nullptr);

        for (auto& kv : frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.model == nullptr) continue;

            PointLightPushConstantData push{};
            push.position = glm::vec4(obj.transform3d.translation, 1.f);
            push.color = glm::vec4(obj.color, obj.pointLight->intensity);
            push.radius = obj.transform3d.scale.x;

            vkCmdPushConstants(
                frameInfo.commandBuffer,
                pipelineLayout,
                VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
                0,
                sizeof(PointLightPushConstantData),
                &push
            );

            vkCmdDraw(frameInfo.commandBuffer, 6, 1, 0, 0);
        }
    }

}