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
    }

    TestApp::~TestApp() { }

    void TestApp::run() {
        // draw frame procedure:
        // 1. Acquire an image from the swap chain, calling vkAcquireNextImageKHR
        // (in SwapChain::acquireNextImage)
        // 2. submit the command buffer to the graphics queue, calling vkQueueSubmit
        // (in SwapChain::submitCommandBuffers)
        // 3. return the image to the swap chain for presentation, calling vkQueuePresentKHR
        // (in SwapChain::submitCommandBuffers)
        // also update the current frame in flight
        SimpleRenderSystem simpleRenderSystem(device, renderer.getSwapChainRenderPass());
        while (!window.shouldClose()) {
            glfwPollEvents();

            if (auto commandBuffer = renderer.beginFrame()) {
                renderer.beginSwapChainRenderPass(commandBuffer);
                simpleRenderSystem.renderGameObjects(commandBuffer, gameObjects);
                renderer.endSwapChainRenderPass(commandBuffer);
                renderer.endFrame();
            }
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
}