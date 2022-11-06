cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

project(VulkanTutorial)

# it's preferable to use the SDL system library but it's still
# possible to compile it as part of this project to step into its sources
# to better understand how vulkan interacts w/ the windowing system
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/CMakeLists.txt")
    add_subdirectory(external/SDL)
else()
    find_package(SDL2 REQUIRED)
endif()

# Recurse into the tutorial directories
add_subdirectory(tutorial)
