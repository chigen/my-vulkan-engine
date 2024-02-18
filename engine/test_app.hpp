#pragma once

/* 
    This app is to test window function and pipeline function
    It will create a default window and create a pipeline with 
    simple_shader.vert and simple_shader.frag
*/

#include "window.hpp"
#include "pipeline.hpp"

namespace engine {
    class TestApp {
        public:
            static constexpr int WIDTH = 800;
            static constexpr int HEIGHT = 600;

            void run();

        private:
            Window window{WIDTH, HEIGHT, "Test App"};
            Device device{window};
            Pipeline pipeline{device, "shaders/simple_shader.vert.spv", "shaders/simple_shader.frag.spv", 
                Pipeline::defaultPipelineConfigInfo(WIDTH, HEIGHT)};
    };
}