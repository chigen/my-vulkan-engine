#include "model.hpp"

#include <cassert>

namespace engine {
    Model::Model(Device& device, const std::vector<Vertex>& vertices) : device{device} {
        createVertexBuffers(vertices);
    }

    Model::~Model(){
        vkDestroyBuffer(device.device(), vertexBuffer, nullptr);
        vkFreeMemory(device.device(), vertexBufferMemory, nullptr);
    }

    void Model::createVertexBuffers(const std::vector<Vertex>& vertices){
        /* 
        map the memory to the vertex buffer,
        copy the data in cpu (vertices) to the gpu memory space
        access the data in gpu with the vertex buffer
        
        Host (cpu) | Device (gpu)
        ------------|--------------
        vertices
        | memcpy()
        v
        void *data      | vertex buffer memory
        vkMapMemory()   | 
        */
        vertexCount = static_cast<uint32_t>(vertices.size());
        assert(vertexCount >= 3 && "Vertex count must be at least 3");

        VkDeviceSize bufferSize = sizeof(vertices[0]) * vertexCount;
        // call createBuffer from device.hpp
        device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            vertexBuffer,
            vertexBufferMemory
        );

        // map the vertex buffer memory to the vertex buffer
        void* data;
        vkMapMemory(device.device(), vertexBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), (size_t) bufferSize);
        vkUnmapMemory(device.device(), vertexBufferMemory);
    }

    void Model::bind(VkCommandBuffer commandBuffer){
        /* 
        call the model bind function after the pipeline bind function
         */
        VkBuffer buffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);
    }

    void Model::draw(VkCommandBuffer commandBuffer){
        vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
    }

    std::vector<VkVertexInputBindingDescription> Model::Vertex::getBindingDescription(){
        // set binding to 0
        std::vector<VkVertexInputBindingDescription> bindingDescriptions(1);
        bindingDescriptions[0].binding = 0;
        bindingDescriptions[0].stride = sizeof(Vertex);
        bindingDescriptions[0].inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return bindingDescriptions;
    }

    std::vector<VkVertexInputAttributeDescription> Model::Vertex::getAttributeDescriptions(){
        // binding 0, location 0, format R32G32_SFLOAT, offset 0
        std::vector<VkVertexInputAttributeDescription> positionAttributeDescription(2);
        positionAttributeDescription[0].binding = 0;
        positionAttributeDescription[0].location = 0;
        positionAttributeDescription[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        positionAttributeDescription[0].offset = offsetof(Vertex, position);
        // binding 0, location 1, format R32G32B32_SFLOAT
        positionAttributeDescription[1].binding = 0;
        positionAttributeDescription[1].location = 1;
        positionAttributeDescription[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        positionAttributeDescription[1].offset = offsetof(Vertex, color);
        return {positionAttributeDescription};
    }
}