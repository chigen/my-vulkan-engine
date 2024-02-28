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
                glm::vec3 normal{};
                glm::vec2 uv{};
                // binding description
                static std::vector<VkVertexInputBindingDescription> getBindingDescription();
                // attribute descriptions
                static std::vector<VkVertexInputAttributeDescription> getAttributeDescriptions();

                bool operator==(const Vertex& other) const
                {
                    return position == other.position && color == other.color && normal == other.normal && uv == other.uv;
                }
            };

            struct Builder
            {
                std::vector<Vertex> vertices{};
                std::vector<uint32_t> indices{};

                void loadModel(const std::string& filePath);
            };
            
            Model(Device& device, const Builder& builder);
            ~Model();

            // delete copy constructor and operator to avoid copying the model
            Model(const Model&) = delete;
            Model& operator=(const Model&) = delete;
            
            static std::unique_ptr<Model> createModelFromFile(Device& device, const std::string& filePath);

            void bind(VkCommandBuffer commandBuffer);
            void draw(VkCommandBuffer commandBuffer);

        private:
            void createVertexBuffers(const std::vector<Vertex>& vertices);
            void createIndexBuffers(const std::vector<uint32_t>& indices);

            Device& device;
            VkBuffer vertexBuffer;
            VkDeviceMemory vertexBufferMemory;
            uint32_t vertexCount;

            bool hasIndexBuffer = false;
            VkBuffer indexBuffer;
            VkDeviceMemory indexBufferMemory;
            uint32_t indexCount;
    };
}