#pragma once

#include <glm/glm.hpp>
#include <vector>

#include "device.hpp"

namespace engine{
    class Model{
        public:
            struct Vertex
            {
                glm::vec3 position{};
                glm::vec3 color{};
                // binding description
                static std::vector<VkVertexInputBindingDescription> getBindingDescription();
                // attribute descriptions
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();
            };
            Model(Device& device, const std::vector<Vertex>& vertices);
            ~Model();

            // delete copy constructor and operator to avoid copying the model
            Model(const Model&) = delete;
            Model& operator=(const Model&) = delete;
            
            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            void createVertexBuffers(const std::vector<Vertex>& vertices);

            Device& device;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;
    };
}