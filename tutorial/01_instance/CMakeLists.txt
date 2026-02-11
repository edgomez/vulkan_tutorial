project(VulkanTutorial01
    LANGUAGES CXX
)

find_package(Vulkan REQUIRED)

add_executable(VulkanTutorial01
    01_instance.cpp
)

target_link_libraries(VulkanTutorial01
    PRIVATE
        VulkanTutorialCompileOptions
        VulkanTutorialCommon
        Vulkan::Vulkan
        SDL3::SDL3
)

target_compile_features(VulkanTutorial01
    PRIVATE
        cxx_std_11
)

set_target_properties(VulkanTutorial01
    PROPERTIES
        OUTPUT_NAME vulkan-tutorial-01-instance
)

install(TARGETS VulkanTutorial01 RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")