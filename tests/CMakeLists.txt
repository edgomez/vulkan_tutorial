project(VulkanTutorialTests
    LANGUAGES CXX
)

include(GoogleTest)

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

target_compile_features(VulkanTutorialTests
    PRIVATE
        cxx_std_11
)

set_target_properties(VulkanTutorialTests
    PROPERTIES
        OUTPUT_NAME vulkan-tutorial-tests
)

enable_testing()
gtest_discover_tests(VulkanTutorialTests)

install(TARGETS VulkanTutorialTests RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
