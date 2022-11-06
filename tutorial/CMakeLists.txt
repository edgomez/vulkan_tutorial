cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# As for our tutorial code, I'd like its code to be in shape
if (   CMAKE_CXX_COMPILER_ID MATCHES ".*LLVM.*"
    OR CMAKE_CXX_COMPILER_ID MATCHES ".*Clang.*"
    OR CMAKE_CXX_COMPILER_ID MATCHES "GNU")
    add_compile_options("-Wall" "-Wextra" "-Werror")
elseif(CMAKE_CXX_COMPILER_ID MATCHES "MSVC")
    add_compile_options("/W3" "/WX")
endif()

add_subdirectory(00_common)
add_subdirectory(01_instance)