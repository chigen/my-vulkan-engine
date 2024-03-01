#pragma once

/* 
    Note:
    For one application, it can contain multiple render systems,
    but one renderer

    In one render system, it contains：
    the pipeline, pipeline layout,
    some simple push constant struct, 
    and game objects

    In renderer, it contains the configuration of:
    the swap chain,
    command buffers,
    draw frame function,

*/

/* 
    Pipeline: defines the entire rendering process from input 
    vertices to final pixel output to the framebuffer

    Pipeline layout: defines the organizational structure of 
    the resources required by the shader in the pipeline, 
    including uniform buffers (Uniform Buffers), samplers 
    (Samplers), images (Images), etc. It describes how these 
    resources are accessed by shaders through Descriptor Set 
    Layouts. Pipeline layout allows you to reuse the same 
    resource organization across different pipelines, even if 
    those pipelines execute different shader code.
*/

#include "window.hpp"
#include "device.hpp"
#include "game_object.hpp"
#include "simple_render_system.hpp"
#include "renderer.hpp"
#include "camera.hpp"
#include "descriptors.hpp"

#include <memory>
#include <vector>
#include <cassert>
#include <stdexcept>

namespace engine {
    class TestApp {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            TestApp();
            ~TestApp();

            TestApp(const TestApp&) = delete;
            TestApp& operator=(const TestApp&) = delete;

            void run();

        private:
            void loadGameObjects();

            Window window{WIDTH, HEIGHT, "Test App"};
            Device device{window};
            Renderer renderer{device, window};

            std::unique_ptr<DescriptorPool> globalPool;
            GameObject::Map gameObjects;
    };
}