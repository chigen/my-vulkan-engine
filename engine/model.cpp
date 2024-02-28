#include "model.hpp"
#include "utils.hpp"

#include <tiny_obj_loader.h>
// GLM_GTX is an experimental extension
#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/hash.hpp>

#include <cassert>
#include <unordered_map>
#include <iostream>

namespace std{
    // create hash value for Vertex struct
    template<> struct hash<engine::Model::Vertex>{
        size_t operator()(engine::Model::Vertex const& vertex) const{
            size_t seed = 0;
            engine::hashCombine(seed, vertex.position, vertex.color, vertex.normal, vertex.uv);
            return seed;
        }
    };

}

namespace engine {
    void Model::Builder::loadModel(const std::string& filePath){
        /* 
        This function will call the tinyobjloader to load the .obj model from the file path
         */
        tinyobj::attrib_t attrib;
        std::vector<tinyobj::shape_t> shapes;
        std::vector<tinyobj::material_t> materials;
        std::string warn;
        std::string err;

        if(!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, filePath.c_str())){
            throw std::runtime_error(warn + err);
        }

        std::unordered_map<Vertex, uint32_t> uniqueVertices{};
        for(const auto& shape: shapes){
            for(const auto& index : shape.mesh.indices){
                Vertex vertex{};
                if(index.vertex_index >= 0){
                    vertex.position = {
                        attrib.vertices[3 * index.vertex_index + 0],
                        attrib.vertices[3 * index.vertex_index + 1],
                        attrib.vertices[3 * index.vertex_index + 2]
                    };

                    vertex.color = {
                        attrib.colors[3 * index.vertex_index + 0],
                        attrib.colors[3 * index.vertex_index + 1],
                        attrib.colors[3 * index.vertex_index + 2]
                    };
                }

                if(index.normal_index >= 0){
                    vertex.normal = {
                        attrib.normals[3 * index.normal_index + 0],
                        attrib.normals[3 * index.normal_index + 1],
                        attrib.normals[3 * index.normal_index + 2]
                    };
                }

                if(index.texcoord_index >= 0){
                    vertex.uv = {
                        attrib.texcoords[2 * index.texcoord_index + 0],
                        attrib.texcoords[2 * index.texcoord_index + 1],
                    };
                }

                // check if the vertex is new => if it's in the uniqueVertices map
                // if it's new, add it to the uniqueVertices map and the vertices vector to record its index
                if(uniqueVertices.count(vertex) == 0){
                    uniqueVertices[vertex] = static_cast<uint32_t>(vertices.size());
                    vertices.push_back(vertex);
                }
                indices.push_back(uniqueVertices[vertex]);
            }

        }
    }
    

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

    std::unique_ptr<Model> Model::createModelFromFile(Device& device, const std::string& filePath){
        // initialize the Model instance with the builder, using unique_ptr
        Model::Builder builder{};
        builder.loadModel(filePath);
        
        std::cout<< "vertices size: " << builder.vertices.size() << std::endl;
        
        return std::make_unique<Model>(device, builder);
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