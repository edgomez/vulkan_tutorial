cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

include(GNUInstallDirs)

project(VulkanTutorial00
    LANGUAGES CXX
)

set(INC_DIR "include/egomez/vulkan_tutorial")
set(SRC_DIR "src")

add_library(VulkanTutorialCommon
    ${SRC_DIR}/scope_guard.cpp
    ${INC_DIR}/scope_guard.h

    ${SRC_DIR}/sdl_helpers.cpp
    ${INC_DIR}/sdl_helpers.h
)

target_link_libraries(VulkanTutorialCommon
    PUBLIC
        SDL2::SDL2
)

target_compile_features(VulkanTutorialCommon
    PUBLIC
        cxx_std_11
)

target_include_directories(VulkanTutorialCommon
    PUBLIC
        "$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>"
        "$<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>"
)

install(TARGETS VulkanTutorialCommon LIBRARY DESTINATION "${CMAKE_INSTALL_LIBRARYDIR}")
install(DIRECTORY include DESTINATION "${CMAKE_INSTALL_INCLUDEDIR}")
