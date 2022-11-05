/* --------------------------------------------------------------------------
 * Vulkan Tutorial 01
 *
 * Getting a vulkan instance and pick up a device
 *
 * See:
 * https://vulkan-tutorial.com/Overview#page_Step-1-Instance-and-physical-device-selection
 *
 * SPDX-FileCopyrightText: 2022 Edouard Gomez
 * SPDX-License-Identifier: MIT
 * ----------------------------------------------------------------------- */

#include <cstdio>
#include <cstdlib>

#include <SDL2/SDL.h>

#include <egomez/vulkan_tutorial/sdl_helpers.h>

int
main(int argc, char** argv)
{
    using namespace egomez::vulkan_tutorial;

    int res;

    res = SDL_Init(SDL_INIT_EVERYTHING);
    if (res) {
        return EXIT_FAILURE;
    }

    unique_sdl_library library{ nullptr };

    unique_sdl_window window{ SDL_CreateWindow(
      "Tutorial 1 - Creating an instance", 0, 0, 320, 240, SDL_WINDOW_VULKAN) };
    if (!window) {
        return EXIT_FAILURE;
    }

    unique_sdl_renderer renderer{ SDL_CreateRenderer(
      window.get(), 0, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC) };
    if (!renderer) {
        return EXIT_FAILURE;
    }

    bool must_quit = false;
    while (!must_quit) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
                case SDL_QUIT:
                    must_quit = true;
                    break;
                default:
                    break;
            }
        }

        res = SDL_RenderClear(renderer.get());
        if (res) {
            return EXIT_FAILURE;
        }
        SDL_RenderPresent(renderer.get());
    }

    return EXIT_SUCCESS;
}
