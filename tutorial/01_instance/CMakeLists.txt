cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

include(GNUInstallDirs)

find_package(Vulkan REQUIRED)

project(VulkanTutorial01
    LANGUAGES CXX
)

add_executable(VulkanTutorial01
    01_instance.cpp
)

target_link_libraries(VulkanTutorial01
    PRIVATE
        VulkanTutorialCommon
        Vulkan::Vulkan
        SDL2::SDL2
)

target_compile_features(VulkanTutorial01
    PUBLIC
        cxx_std_11
)

set_target_properties(VulkanTutorial01
    PROPERTIES
        OUTPUT_NAME vulkan-tutorial-01-instance
)

install(TARGETS VulkanTutorial01 RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")