#include "point_light_system.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <map>

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
        // enable alpha blending
        Pipeline::enableAlphaBlending(pipelineConfig);

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

    glm::mat4 PointLightSystem::createRotations(int axis, float rotationSpeed) {
        if (axis == 1)
            // rotate around y axis
            return glm::rotate(glm::mat4(1.f), rotationSpeed, {0.f, -1.f, 0.f});
        
        else if(axis == 2){
            // rotate around z axis
            glm::mat4 translateToOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.f, 0.0f));
            glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.f, 0.0f));
            glm::mat4 rotateZ = glm::rotate(glm::mat4(1.0f), rotationSpeed, glm::vec3(0.0f, 0.0f, -1.0f));
            return translateBack * rotateZ * translateToOrigin;
        }
        
        else if(axis == 0) {
            // rotate around x axis
            glm::mat4 translateToOrigin = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.f, 0.0f));
            glm::mat4 translateBack = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, -2.f, 0.0f));
            glm::mat4 rotateX = glm::rotate(glm::mat4(1.0f), rotationSpeed, glm::vec3(-1.0f, 0.0f, 0.0f));
            return translateBack * rotateX * translateToOrigin;
        }      
    }

    void PointLightSystem::update(FrameInfo& frameInfo, GlobalUbo& ubo) {
        // periodically update the point light's properties

        int lightIndex = 0;
        for (auto& kv : frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.pointLight == nullptr) continue;

            assert(lightIndex < MAX_POINT_LIGHTS && "Point lights exceed maximum specified");

            // update light position
            auto rotateLight = createRotations(lightIndex%3, 0.5f * frameInfo.frameTime);
            obj.transform3d.translation = glm::vec3(rotateLight * glm::vec4(obj.transform3d.translation, 1.f));

            // copy light to ubo
            ubo.pointLights[lightIndex].position = glm::vec4(obj.transform3d.translation, 1.f);
            ubo.pointLights[lightIndex].color = glm::vec4(obj.color, obj.pointLight->intensity);

            lightIndex += 1;
        }
        ubo.numLights = lightIndex;
    }

    void PointLightSystem::render(FrameInfo& frameInfo){
        // sort the lights
        std::map<float, GameObject::id_t> sorted;
        for (auto& kv: frameInfo.gameObjects) {
            auto& obj = kv.second;
            if (obj.pointLight == nullptr) continue;

            auto offset = frameInfo.camera.getPosition() - obj.transform3d.translation;
            float dis = glm::dot(offset, offset);
            sorted[dis] = obj.getId();
        }

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

        // for transparent objects, we need to render them from back to front
        for (auto it=sorted.rbegin(); it!=sorted.rend(); ++it) {
            auto& obj = frameInfo.gameObjects.at(it->second);

            if (obj.pointLight == nullptr) continue;

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