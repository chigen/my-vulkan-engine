#include "model.hpp"

#include <cassert>

namespace engine {
    Model::Model(Device& device, const Builder& builder) : device{device} {
        createVertexBuffers(builder.vertices);
        createIndexBuffers(builder.indices);
    }

    Model::~Model(){
        vkDestroyBuffer(device.device(), vertexBuffer, nullptr);
        vkFreeMemory(device.device(), vertexBufferMemory, nullptr);

        if(hasIndexBuffer){
            vkDestroyBuffer(device.device(), indexBuffer, nullptr);
            vkFreeMemory(device.device(), indexBufferMemory, nullptr);
        }
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

        // use staging buffer for current static data to speed up
        /* 
        staging buffer is a buffer that is used as a temporary buffer to copy data from the cpu to the gpu
        Host (cpu)      | Device (gpu)
        ----------------|------------------
        vertices
        | memcpy()
        v
        void *data      | staging buffer memory
        vkMapMemory()   |        | copyBuffer()
                        |         v
                        | vertex/ index buffer memory
                        | (device local memory, faster)
         */
        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        // call createBuffer from device.hpp
        device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory
        );

        // map the vertex buffer memory to the staging buffer
        void* data;
        vkMapMemory(device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, vertices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(device.device(), stagingBufferMemory);

        // create the vertex buffer and copy data from staging buffer to vertex buffer
        device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_VERTEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            vertexBuffer,
            vertexBufferMemory
        );

        device.copyBuffer(stagingBuffer, vertexBuffer, bufferSize);

        // free the staging buffer and its memory
        vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
        vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
    }

        void Model::createIndexBuffers(const std::vector<uint32_t>& indices){
        /* 
        same logic as createVertexBuffers
         */
        indexCount = static_cast<uint32_t>(indices.size());
        hasIndexBuffer = indexCount > 0;

        if (!hasIndexBuffer) return;

        VkDeviceSize bufferSize = sizeof(indices[0]) * indexCount;

        VkBuffer stagingBuffer;
        VkDeviceMemory stagingBufferMemory;

        // call createBuffer from device.hpp
        device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT,
            VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT,
            stagingBuffer,
            stagingBufferMemory
        );

        // map the vertex buffer memory to the staging buffer
        void* data;
        vkMapMemory(device.device(), stagingBufferMemory, 0, bufferSize, 0, &data);
        memcpy(data, indices.data(), static_cast<size_t>(bufferSize));
        vkUnmapMemory(device.device(), stagingBufferMemory);

        // create the vertex buffer and copy data from staging buffer to vertex buffer
        device.createBuffer(
            bufferSize,
            VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_INDEX_BUFFER_BIT,
            VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
            indexBuffer,
            indexBufferMemory
        );

        device.copyBuffer(stagingBuffer, indexBuffer, bufferSize);

        // free the staging buffer and its memory
        vkDestroyBuffer(device.device(), stagingBuffer, nullptr);
        vkFreeMemory(device.device(), stagingBufferMemory, nullptr);
    }

    void Model::bind(VkCommandBuffer commandBuffer){
        /* 
        call the model bind function after the pipeline bind function
         */
        VkBuffer buffers[] = {vertexBuffer};
        VkDeviceSize offsets[] = {0};
        vkCmdBindVertexBuffers(commandBuffer, 0, 1, buffers, offsets);

        if (hasIndexBuffer){
            vkCmdBindIndexBuffer(commandBuffer, indexBuffer, 0, VK_INDEX_TYPE_UINT32);
        }
    }

    void Model::draw(VkCommandBuffer commandBuffer){
        if(hasIndexBuffer){
            vkCmdDrawIndexed(commandBuffer, indexCount, 1, 0, 0, 0);
        }
        else{
            vkCmdDraw(commandBuffer, vertexCount, 1, 0, 0);
        }
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