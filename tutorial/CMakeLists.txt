cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

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

add_subdirectory(00_common)
add_subdirectory(01_instance)