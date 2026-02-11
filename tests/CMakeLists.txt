cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

add_executable(VulkanTutorialTests
    main.cpp
    test_scope_guard.cpp
)

target_link_libraries(VulkanTutorialTests
    PRIVATE
        VulkanTutorialCompileOptions

        VulkanTutorialCommon
        GTest::gtest
)

set_target_properties(VulkanTutorialTests
    PROPERTIES
        OUTPUT_NAME "vulkan-tutorial-tests"
)