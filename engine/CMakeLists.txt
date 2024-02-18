message("\n-- Engine --\n")

# No need to include_directories("../") since it's better to use target_include_directories on specific targets

file(GLOB ENGINE_SOURCE_FILES *.cpp *.hpp) # Adding *.hpp to ensure IDEs recognize headers
add_library(engine ${ENGINE_SOURCE_FILES})

# Specify include directories for this target
target_include_directories(engine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

# If the engine library needs to link against Vulkan or other libraries, specify it here
target_link_libraries(engine PUBLIC Vulkan::Vulkan glfw)