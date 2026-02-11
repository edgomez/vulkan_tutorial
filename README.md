# Vulkan Tutorial

- [Vulkan Tutorial](#vulkan-tutorial)
  - [Description](#description)
  - [Tutorials](#tutorials)
  - [Requirements](#requirements)
  - [How to build](#how-to-build)
  - [Running tutorials](#running-tutorials)
  - [Running tests](#running-tests)
  - [Code structure](#code-structure)
  - [Code style](#code-style)
  - [Editing the code with Visual Studio Code](#editing-the-code-with-visual-studio-code)
  - [License](#license)

## Description

This is my attempt at following the [Vulkan Tutorial](https://vulkan-tutorial.com/) by implementing each step from scratch rather than copying existing code.

## Tutorials

- **00_common** - Common library with RAII helpers for SDL and scope guards
- **01_instance** - [Creating a Vulkan instance and selecting a physical device](tutorial/01_instance/README.md)

## Requirements

I won't assume much about your development environment, but the code
contained in this repository expects the following software to be
installed:

1. SDL3 development package (can be installed system-wide, or bootstrapped locally by running `git submodule update --init external/SDL` to fetch SDL3 into the `external/` directory)
2. Vulkan development package (must be installed system-wide)
3. Validation layer development package (optional, but highly recommended for debug messages and error checking)
4. CMake >= 3.21 (not a hard requirement, but I know this version will work for sure)
5. A C++11 compiler (clang, g++ are fine, msvc may work)
6. Google Test (optional, for running unit tests; can be installed system-wide or bootstrapped locally by running `git submodule update --init external/googletest`)

## How to build

```sh
cmake -B build/host -S . -G "Ninja Multi-Config"
cmake --build build/host --config RelWithDebInfo --target all
```

You can choose any generator that fits your development environment.

## Running tutorials

After building, executables are located in `build/host/tutorial/<chapter>/<config>/`:

```sh
# Run tutorial 01 with debug validation layers
./build/host/tutorial/01_instance/RelWithDebInfo/vulkan-tutorial-01-instance --debug

# Run with specific device
./build/host/tutorial/01_instance/RelWithDebInfo/vulkan-tutorial-01-instance --device "NVIDIA GeForce RTX 3080"

# Custom window size
./build/host/tutorial/01_instance/RelWithDebInfo/vulkan-tutorial-01-instance --width 1920 --height 1080
```

## Running tests

Tests are built by default when building this project directly (as top-level). Google Test must be available either system-wide or via submodule:

```sh
# Initialize Google Test submodule if not using system package
git submodule update --init external/googletest

# Build (tests included by default)
cmake -B build/host -S . -G "Ninja Multi-Config"
cmake --build build/host --config RelWithDebInfo --target all

# Run all tests
./build/host/tests/RelWithDebInfo/vulkan-tutorial-tests

# Run tests with verbose output
./build/host/tests/RelWithDebInfo/vulkan-tutorial-tests --gtest_verbose
```

To disable tests:

```sh
cmake -B build/host -S . -G "Ninja Multi-Config" -DVULKAN_TUTORIAL_BUILD_TESTS=OFF
```

If using this project via `add_subdirectory()`, tests are disabled by default. Enable them with `-DVULKAN_TUTORIAL_BUILD_TESTS=ON`.

## Code structure

```text
.
├── CMakeLists.txt              # Top-level build configuration
├── external/                   # Third-party dependencies (SDL3, Google Test)
├── tests/                      # Unit tests (built if Google Test is available)
├── tutorial/
│   ├── 00_common/              # VulkanTutorialCommon library
│   └── 01_instance/            # Tutorial 01: Instance and device
└── script/                     # Utility scripts (formatting, etc.)
```

Each tutorial's `CMakeLists.txt` is standalone except for its dependency on `VulkanTutorialCommon`. The code is written in C++11, following modern CMake guidelines. Headers are self-sufficient - each has a corresponding `.cpp` file that includes it first to verify independence.

## Code style

This project uses clang-format for consistent code styling. Apply formatting using the `.clang-format` file at the repository root.

## Editing the code with Visual Studio Code

Recommended extensions for VS Code:

- **C/C++** (`ms-vscode.cpptools`) - Microsoft
- **Better C++ Syntax** (`jeff-hykin.better-cpp-syntax`) - Jeff Hykin
- **CMake Tools** (`ms-vscode.cmake-tools`) - Microsoft
- **CMake Language Support** (`twxs.cmake`) - twxs

These provide code completion, browsing, formatting, and build integration.

## License

The code found in this repository is licensed under the terms of the
MIT license. See the [LICENSE.md](./LICENSE.md) file for the exact terms.
