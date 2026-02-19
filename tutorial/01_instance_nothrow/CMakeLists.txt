project(VulkanTutorial01NoThrow
    LANGUAGES CXX
)

find_package(Vulkan REQUIRED)

add_executable(VulkanTutorial01NoThrow
    01_instance_nothrow.cpp
)

target_link_libraries(VulkanTutorial01NoThrow
    PRIVATE
        VulkanTutorialCompileOptions
        VulkanTutorialCommon
        Vulkan::Vulkan
        SDL3::SDL3
)

target_compile_features(VulkanTutorial01NoThrow
    PRIVATE
        cxx_std_11
)

if (MSVC)
    target_compile_options(VulkanTutorial01NoThrow
        PRIVATE
            "/EHs-c-"
    )
    target_compile_definitions(VulkanTutorial01NoThrow
        PRIVATE
            "_HAS_EXCEPTIONS=0"
    )
elseif (CMAKE_CXX_COMPILER_ID MATCHES ".*LLVM.*"
    OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*"
    OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(VulkanTutorial01NoThrow
        PRIVATE
            "-fno-exceptions"
    )
endif()

set_target_properties(VulkanTutorial01NoThrow
    PROPERTIES
        OUTPUT_NAME vulkan-tutorial-01-instance-nothrow
)

install(TARGETS VulkanTutorial01NoThrow RUNTIME DESTINATION "${CMAKE_INSTALL_BINDIR}")
