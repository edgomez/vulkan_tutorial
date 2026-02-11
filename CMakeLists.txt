cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(VulkanTutorial
    LANGUAGES C CXX
    VERSION 0.0.1
)

# ---------------------------------------------------------------------------
# External dependencies and cmake modules
# ---------------------------------------------------------------------------

# it's preferable to use the SDL system library but it's still
# possible to compile it as part of this project to step into its sources
# to better understand how vulkan interacts w/ the windowing system
if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/CMakeLists.txt")
    add_subdirectory(external/SDL)
else()
    find_package(SDL3 REQUIRED)
endif()

if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/CMakeLists.txt")
    add_subdirectory(external/googletest)
else()
    find_package(GTest)
endif()

include(GNUInstallDirs)

# ---------------------------------------------------------------------------
# Compilation options common to the tutorial/tests code
# ---------------------------------------------------------------------------

add_library(VulkanTutorialCompileOptions INTERFACE)
# As for our tutorial code, I'd like its code to be in shape
if (   CMAKE_CXX_COMPILER_ID MATCHES ".*LLVM.*"
    OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*"
    OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    target_compile_options(VulkanTutorialCompileOptions
        INTERFACE
            "-Wall" "-Wextra" "-Werror"
            "-Wpedantic" "-Wshadow" "-Wnon-virtual-dtor" "-Wold-style-cast" "-Wcast-align" "-Woverloaded-virtual"
            "-Wconversion" "-Wsign-conversion" "-Wnull-dereference" "-Wdouble-promotion" "-Wformat=2"
    )
    if(CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*")
        target_compile_options(VulkanTutorialCompileOptions
            INTERFACE
                "-Wdocumentation" "-Wno-c++98-compat" "-Wno-c++98-compat-pedantic"
        )
    endif()
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    target_compile_options(VulkanTutorialCompileOptions
        INTERFACE
            "/W4" "/WX" "/permissive-" "/we4251" "/we4275" "/we4661"
    )
endif()

# ---------------------------------------------------------------------------
# Recurse into the code directories
# ---------------------------------------------------------------------------

add_subdirectory(tutorial)

option(VULKAN_TUTORIAL_BUILD_TESTS "Whether to build the tests" ${PROJECT_IS_TOP_LEVEL})
if (TARGET GTest::gtest AND VULKAN_TUTORIAL_BUILD_TESTS)
    add_subdirectory(tests)
endif()
