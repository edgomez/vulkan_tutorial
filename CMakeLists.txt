cmake_minimum_required(VERSION 3.21 FATAL_ERROR)

project(VulkanTutorial
    LANGUAGES C CXX
    VERSION 0.0.1
)

# ---------------------------------------------------------------------------
# External dependencies and cmake modules
# ---------------------------------------------------------------------------

set(VULKAN_TUTORIAL_SDL3_PROVIDER "AUTO" CACHE STRING
    "SDL3 dependency provider: AUTO, SYSTEM, or EXTERNAL"
)
set_property(CACHE VULKAN_TUTORIAL_SDL3_PROVIDER PROPERTY STRINGS AUTO SYSTEM EXTERNAL)

set(VULKAN_TUTORIAL_GTEST_PROVIDER "AUTO" CACHE STRING
    "GoogleTest dependency provider: AUTO, SYSTEM, or EXTERNAL"
)
set_property(CACHE VULKAN_TUTORIAL_GTEST_PROVIDER PROPERTY STRINGS AUTO SYSTEM EXTERNAL)

if (NOT VULKAN_TUTORIAL_SDL3_PROVIDER MATCHES "^(AUTO|SYSTEM|EXTERNAL)$")
    message(FATAL_ERROR
        "Invalid VULKAN_TUTORIAL_SDL3_PROVIDER='${VULKAN_TUTORIAL_SDL3_PROVIDER}'. "
        "Expected one of: AUTO, SYSTEM, EXTERNAL"
    )
endif()

if (NOT VULKAN_TUTORIAL_GTEST_PROVIDER MATCHES "^(AUTO|SYSTEM|EXTERNAL)$")
    message(FATAL_ERROR
        "Invalid VULKAN_TUTORIAL_GTEST_PROVIDER='${VULKAN_TUTORIAL_GTEST_PROVIDER}'. "
        "Expected one of: AUTO, SYSTEM, EXTERNAL"
    )
endif()

message(STATUS "SDL3 provider mode: ${VULKAN_TUTORIAL_SDL3_PROVIDER}")
message(STATUS "GTest provider mode: ${VULKAN_TUTORIAL_GTEST_PROVIDER}")

# it's preferable to use the SDL system library but it's still
# possible to compile it as part of this project to step into its sources
# to better understand how vulkan interacts w/ the windowing system
if (VULKAN_TUTORIAL_SDL3_PROVIDER STREQUAL "EXTERNAL")
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/CMakeLists.txt")
        message(STATUS "SDL3 provider resolved: EXTERNAL (external/SDL)")
        add_subdirectory(external/SDL)
    else()
        message(FATAL_ERROR
            "VULKAN_TUTORIAL_SDL3_PROVIDER=EXTERNAL but external/SDL is missing. "
            "Initialize it with: git submodule update --init external/SDL"
        )
    endif()
elseif (VULKAN_TUTORIAL_SDL3_PROVIDER STREQUAL "SYSTEM")
    message(STATUS "SDL3 provider resolved: SYSTEM (find_package)")
    find_package(SDL3 REQUIRED)
else()
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/SDL/CMakeLists.txt")
        message(STATUS "SDL3 provider resolved: EXTERNAL (auto-detected external/SDL)")
        add_subdirectory(external/SDL)
    else()
        message(STATUS "SDL3 provider resolved: SYSTEM (auto-detected find_package)")
        find_package(SDL3 REQUIRED)
    endif()
endif()

if (VULKAN_TUTORIAL_GTEST_PROVIDER STREQUAL "EXTERNAL")
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/CMakeLists.txt")
        message(STATUS "GTest provider resolved: EXTERNAL (external/googletest)")
        add_subdirectory(external/googletest)
    else()
        message(FATAL_ERROR
            "VULKAN_TUTORIAL_GTEST_PROVIDER=EXTERNAL but external/googletest is missing. "
            "Initialize it with: git submodule update --init external/googletest"
        )
    endif()
elseif (VULKAN_TUTORIAL_GTEST_PROVIDER STREQUAL "SYSTEM")
    message(STATUS "GTest provider resolved: SYSTEM (find_package REQUIRED)")
    find_package(GTest REQUIRED)
else()
    if (EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/googletest/CMakeLists.txt")
        message(STATUS "GTest provider resolved: EXTERNAL (auto-detected external/googletest)")
        add_subdirectory(external/googletest)
    else()
        message(STATUS "GTest provider resolved: SYSTEM/UNAVAILABLE (auto-detected find_package)")
        find_package(GTest)
    endif()
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
