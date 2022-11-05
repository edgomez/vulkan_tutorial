# Vulkan Tutorial

## Description

This is my attempt at following the [Vulkan Tutorial](https://vulkan-tutorial.com/),
but I won't just compile their GitHub code.

Walking the path is probably going to be more fruitful on the long term.

## Requirements

I won't assume much about your development environment, but the code
contained in this repository expects the following software to be
installed:

1. SDL2 development package
2. Vulkan development package
3. Validation layer development package
4. CMake >= 3.20 (not a hard requirement, but I know this version will work for sure)
5. A C++11 compiler (clang, g++ are fine, msvc may work)

## How to build

```console
cd "${vulkan_tutorial_root}"
cmake -B build/host -S . -G "Ninja Multi-Config" -DCMAKE_BUILD_TYPE=RelWithDebInfo
cmake --build build/host --target all
```

Of course you can decide the generator that best fits your
development environment.

## Code structure

The top layer `CMakeLists.txt` just recurses into the `tutorial` directory
that then descends into each chapter.

Note I added a `00_common` chapter that implements a `VulkanTutorialCommon`
library to factor pieces of code that are used in more than one chapter.

Each `CMakeLists.txt` tutorial file is written in a way that makes it
standalone except for its dependency on the `VulkanTutorialCommon`
library which is found in `00_common`

The code is written in c++ 11, and the CMake code tries to apply modern
CMake guidelines.

I tend to add c++ files that just include their header counter part. This
aims at ensuring the headers are self sufficient by including them first
in the c++ compilation unit.

Also note that `install()` instructions are added even though i doubt
there is any value in ever installing the resulting compilation artefacts.

The code is the interesting part in this repository, not the resulting
binaries/headers.

## Code style

I don't like discussing coding style, I use clang-format to style my
code... it does it relatively well, and I don't like to enforce a style
manually</lazy programmer>

Please apply the style as using the file `.clang-format` located at the
root of this repository.

## Editing the code with Visual Studio Code

That's what I personally use, but it's not mandatory. If you do so,
I advise you installing the following extensions:

- C/C++ from Microsoft
- Better C++ syntax from Jeff Hykin
- CMake Tools from Microsoft
- CMake Language Support from Jose Torres

With these, you can configure/build/edit the code comfortably w/ code
completion/browsing/formatting etc...

## License

The code found in this repository is licensed under the terms of the
MIT license. See the [LICENSE](./LICENSE) file for the exact terms.
