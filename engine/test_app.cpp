#include "test_app.hpp"
#include "keyboard_controller.hpp"
#include "buffer.hpp"

// libs
#define GLM_FORCE_RADIANS
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include <glm/glm.hpp>
#include <glm/gtc/constants.hpp>

#include <chrono>

namespace engine {
    

    TestApp::TestApp() {
        // create global descriptor pool
        globalPool = DescriptorPool::Builder(device)
            .setMaxSets(SwapChain::MAX_FRAMES_IN_FLIGHT)
            .addPoolSize(VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, SwapChain::MAX_FRAMES_IN_FLIGHT)
            .build();

        loadGameObjects();
    }

    TestApp::~TestApp() { }

    void TestApp::run() {
        std::vector<std::unique_ptr<Buffer>> uboBuffers(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i=0; i<uboBuffers.size(); ++i){
            uboBuffers[i] = std::make_unique<Buffer>(
                device,
                sizeof(GlobalUbo),
                1,
                VK_BUFFER_USAGE_UNIFORM_BUFFER_BIT,
                VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT
            );
            uboBuffers[i]->map();
        }

        // an example for descriptor set:
        // bind pipeline, bind global descriptor set -  at set #0
        // for each material in Materials:
            // bind material descriptor set - at set #1

            // for each object using this material:
                // bind object descriptor set - at set #2
                // draw object

        // [Master render system](globalSetLayout)
        //  |
        //  v
        // sub systems: [Simple render system](set 0: globalSetLayout, set 1: xxxSetLayout ...) 
        // [xxx render system](set 0: globalSetLayout, set 1: yyyLayout ...) ...
        auto globalSetLayout = DescriptorSetLayout::Builder(device)
            .addBinding(0, VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER, VK_SHADER_STAGE_ALL_GRAPHICS)
            .build();

        std::vector<VkDescriptorSet> globalDescriptorSets(SwapChain::MAX_FRAMES_IN_FLIGHT);
        for (int i=0; i<globalDescriptorSets.size(); ++i){
            auto bufferInfo = uboBuffers[i]->descriptorInfo();
            DescriptorWriter(*globalSetLayout, *globalPool)
            .writeBuffer(0, &bufferInfo)
            .build(globalDescriptorSets[i]);
        }

        // draw frame procedure:
        // 1. Acquire an image from the swap chain, calling vkAcquireNextImageKHR
        // (in SwapChain::acquireNextImage)
        // 2. submit the command buffer to the graphics queue, calling vkQueueSubmit
        // (in SwapChain::submitCommandBuffers)
        // 3. return the image to the swap chain for presentation, calling vkQueuePresentKHR
        // (in SwapChain::submitCommandBuffers)
        // also update the current frame in flight
        SimpleRenderSystem simpleRenderSystem(device, 
            renderer.getSwapChainRenderPass(), 
            globalSetLayout->getDescriptorSetLayout());
        PointLightSystem pointLightSystem(device, 
            renderer.getSwapChainRenderPass(), 
            globalSetLayout->getDescriptorSetLayout());
        Camera camera{};

        // create a camera viewer object
        auto cameraObject = GameObject::createGameObject();
        cameraObject.transform3d.translation.z = -2.5f;
        KeyboardController cameraController{};

        auto currentTime = std::chrono::high_resolution_clock::now();

        std::cout<<"Start running the app"<<std::endl;

        while (!window.shouldClose()) {
            glfwPollEvents();

            auto newTime = std::chrono::high_resolution_clock::now();
            float frameTime = std::chrono::duration<float, std::chrono::seconds::period>(newTime - currentTime).count();
            currentTime = newTime;

            cameraController.moveInXZPlane(window.getGLFWwindow(), frameTime, cameraObject);
            camera.setViewYXZ(cameraObject.transform3d.translation, cameraObject.transform3d.rotation);

            float aspect = renderer.getAspectRatio();
            // camera.setOrthographicProjection(-aspect, aspect, -1.0f, 1.0f, -1.0f, 1.0f);
            camera.setPerspectiveProjection(glm::radians(100.0f), aspect, 0.1f, 100.0f);

            // std::cout<<"Before begining the frame "<<std::endl;

            if (auto commandBuffer = renderer.beginFrame()) {
                int frameIndex = renderer.getFrameIndex();
                FrameInfo frameInfo{
                    frameIndex, 
                    frameTime, 
                    commandBuffer, 
                    camera, 
                    globalDescriptorSets[frameIndex], 
                    gameObjects};

                // update
                GlobalUbo ubo{};
                ubo.project = camera.getProjection();
                ubo.view = camera.getView();
                ubo.inverseView = camera.getInverseView();
                pointLightSystem.update(frameInfo, ubo);

                // std::cout<<"updated point light "<<std::endl;

                uboBuffers[frameIndex]->writeToBuffer(&ubo);
                uboBuffers[frameIndex]->flush();

                renderer.beginSwapChainRenderPass(commandBuffer);
                // std::cout<<"beginned swap chain render pass "<<std::endl;
                simpleRenderSystem.renderGameObjects(frameInfo);
                // std::cout<<"rendered game objects "<<std::endl;
                pointLightSystem.render(frameInfo);
                // std::cout<<"rendered point light "<<std::endl;
                renderer.endSwapChainRenderPass(commandBuffer);
                // std::cout<<"ended swap chain render pass "<<std::endl;
                renderer.endFrame();
            }
        }
        // wait for the device (gpu) to finish before cleaning up
        vkDeviceWaitIdle(device.device());
    }

    // temporary helper function, creates a 1x1x1 cube centered at offset with an index buffer
    std::unique_ptr<Model> createCubeModel(Device& device, glm::vec3 offset) {
        Model::Builder modelBuilder{};
        modelBuilder.vertices = {
            // left face (white)
            {{-.5f, -.5f, -.5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, -.5f, .5f}, {.9f, .9f, .9f}},
            {{-.5f, .5f, -.5f}, {.9f, .9f, .9f}},

            // right face (yellow)
            {{.5f, -.5f, -.5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, -.5f, .5f}, {.8f, .8f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .8f, .1f}},

            // top face (orange, remember y axis points down)
            {{-.5f, -.5f, -.5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{-.5f, -.5f, .5f}, {.9f, .6f, .1f}},
            {{.5f, -.5f, -.5f}, {.9f, .6f, .1f}},

            // bottom face (red)
            {{-.5f, .5f, -.5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{-.5f, .5f, .5f}, {.8f, .1f, .1f}},
            {{.5f, .5f, -.5f}, {.8f, .1f, .1f}},

            // nose face (blue)
            {{-.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{-.5f, .5f, 0.5f}, {.1f, .1f, .8f}},
            {{.5f, -.5f, 0.5f}, {.1f, .1f, .8f}},

            // tail face (green)
            {{-.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{-.5f, .5f, -0.5f}, {.1f, .8f, .1f}},
            {{.5f, -.5f, -0.5f}, {.1f, .8f, .1f}},
        };
        for (auto& v : modelBuilder.vertices) {
            v.position += offset;
        }

        modelBuilder.indices = {0,  1,  2,  0,  3,  1,  4,  5,  6,  4,  7,  5,  8,  9,  10, 8,  11, 9,
                                12, 13, 14, 12, 15, 13, 16, 17, 18, 16, 19, 17, 20, 21, 22, 20, 23, 21};

        return std::make_unique<Model>(device, modelBuilder);
    }
    
    void TestApp::loadGameObjects() {
        // create a model using .obj file
        std::shared_ptr<Model> model = Model::createModelFromFile(device, "../assets/models/room.obj");
        auto gameObj = GameObject::createGameObject();
        gameObj.model = model;
        gameObj.transform3d.translation = {-0.5f, 0.5f, 0.0f};
        gameObj.transform3d.scale = glm::vec3(1.f);
        gameObj.transform3d.rotation = glm::vec3(glm::radians(90.0f), glm::radians(90.0f), 0.f);
        gameObjects.emplace(gameObj.getId(), std::move(gameObj));

        std::shared_ptr<Model> flat_vase_model = Model::createModelFromFile(device, "../assets/models/flat_vase.obj");
        auto flat_vase = GameObject::createGameObject();
        flat_vase.model = flat_vase_model;
        flat_vase.transform3d.translation = {1.0f, 0.5f, 0.0f};
        flat_vase.transform3d.scale = glm::vec3(3.f);
        gameObjects.emplace(flat_vase.getId(), std::move(flat_vase));

        std::shared_ptr<Model> smooth_vase_model = Model::createModelFromFile(device, "../assets/models/smooth_vase.obj");
        auto smooth_vase = GameObject::createGameObject();
        smooth_vase.model = smooth_vase_model;
        smooth_vase.transform3d.translation = {2.0f, 0.5f, 0.0f};
        smooth_vase.transform3d.scale = glm::vec3(3.f);
        gameObjects.emplace(smooth_vase.getId(), std::move(smooth_vase));

        std::shared_ptr<Model> quad_model = Model::createModelFromFile(device, "../assets/models/quad.obj");
        auto quad = GameObject::createGameObject();
        quad.model = quad_model;
        quad.transform3d.translation = {0.0f, 0.5f, 0.0f};
        quad.transform3d.scale = {3.f, 1.f, 3.f};
        gameObjects.emplace(quad.getId(), std::move(quad));

        std::vector<glm::vec3> lightColors{
            {1.f, .1f, .1f},
            {.1f, .1f, 1.f},
            {.1f, 1.f, .1f},
            {1.f, 1.f, .1f},
            {.1f, 1.f, 1.f},
            {1.f, 1.f, 1.f}  
        };
        std::vector<glm::mat4> rotations;
        // rotate around x axis 1
        rotations.push_back(glm::rotate(glm::mat4(1.f), (-0.125f * glm::two_pi<float>()), {0.f, -1.f, 0.f}));
        // rotate around y axis 1
        rotations.push_back(glm::rotate(glm::mat4(1.f), (-0.125f * glm::two_pi<float>()), {0.f, -1.f, 0.f}));
        // rotate around z axis 1
        rotations.push_back(glm::rotate(glm::mat4(1.f), (0.125f * glm::two_pi<float>()), {0.f, -1.f, 0.f}));
        // rotate around x axis 2
        rotations.push_back(glm::rotate(glm::mat4(1.f), (0.375f * glm::two_pi<float>()), {0.f, -1.f, 0.f}));
        // rotate around y axis 2
        rotations.push_back(glm::rotate(glm::mat4(1.f), (0.375f * glm::two_pi<float>()), {0.f, -1.f, 0.f}));
        // rotate around z axis 2
        rotations.push_back(glm::rotate(glm::mat4(1.f), (0.625f * glm::two_pi<float>()), {0.f, -1.f, 0.f}));
        for (int i = 0; i < lightColors.size(); i++) {
            auto pointLight = GameObject::makePointLight(0.2f);
            pointLight.color = lightColors[i];
            auto rotateLight = glm::rotate(
                glm::mat4(.5f),
                (i * glm::two_pi<float>()) / lightColors.size(),
                {0.f, -1.f, 0.f});
            pointLight.transform3d.translation = glm::vec3(rotations[i] * glm::vec4(-1.f, -1.f, -1.f, 1.f));
            pointLight.transform3d.translation.y -= 1.f;
            if(i==0)
                pointLight.transform3d.translation.x -= 0.5f;
            else if(i==3)
                pointLight.transform3d.translation.x += 0.3f;
            // pointLight.transform3d.translation.x -= i * 0.1;
            std::cout << "point light's position:" << pointLight.transform3d.translation.x 
                << " " << pointLight.transform3d.translation.y << " " 
                << pointLight.transform3d.translation.z << std::endl;
            gameObjects.emplace(pointLight.getId(), std::move(pointLight));
        }

        // create a model hard-coded with a cube
        /* std::shared_ptr<Model> cubeModel = createCubeModel(device, {0.0f, 0.0f, 0.0f});
        auto cube = GameObject::createGameObject();
        cube.model = cubeModel;
        // scale and move the cube into available space
        cube.transform3d.translation = {0.0f, 0.0f, 1.5f};
        cube.transform3d.scale = {0.5f, 0.5f, 0.5f};
        gameObjects.push_back(std::move(cube)); */

        // create a model with a triangle
        /* std::vector<Model::Vertex> vertices = {
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

        gameObjects.push_back(std::move(triangle)); */
    }
}