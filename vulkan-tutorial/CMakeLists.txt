cmake_minimum_required(VERSION 3.10)

# Project Name
project(VulkanApp)

# Specify C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find packages
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

# Add the executable
add_executable(VulkanApp main.cpp)

# Include Directories
include_directories(${GLFW_INCLUDE_DIRS} ${GLM_INCLUDE_DIRS} ${Vulkan_INCLUDE_DIRS})

# Link Libraries
target_link_libraries(VulkanApp glfw ${GLM_LIBRARIES} ${Vulkan_LIBRARIES})
