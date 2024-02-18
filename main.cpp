#include "engine/test_app.hpp"
#include <iostream>
#include <stdexcept>
#include <cstdlib>

int main() {
    engine::TestApp app{};

    try {
        app.run();
    } catch (const std::exception& e) {
        std::cerr << e.what() << std::endl;
        return EXIT_FAILURE;
    }
    
    return EXIT_SUCCESS;
}
