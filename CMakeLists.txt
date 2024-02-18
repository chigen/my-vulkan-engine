cmake_minimum_required(VERSION 3.10)

# Project Name
project(ZZYEngine VERSION 0.1)

# Specify C++ Standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Find packages
find_package(glfw3 REQUIRED)
find_package(glm REQUIRED)
find_package(Vulkan REQUIRED)

if(glfw3_FOUND)
    message(STATUS "GLFW3 found")
endif()

if(Vulkan_FOUND)
    message(STATUS "Vulkan found")
endif()

# Include the engine directory
add_subdirectory(engine)

add_executable(ZZYEngine main.cpp)

# It's better to use target_include_directories and target_link_libraries on a target basis
target_include_directories(ZZYEngine PRIVATE ${Vulkan_INCLUDE_DIRS} )
target_link_libraries(ZZYEngine PRIVATE glfw Vulkan::Vulkan engine)

add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
                   COMMAND ${CMAKE_COMMAND} -E copy_directory
                   ${CMAKE_SOURCE_DIR}/shader $<TARGET_FILE_DIR:${PROJECT_NAME}>/shader)