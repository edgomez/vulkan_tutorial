/* --------------------------------------------------------------------------
 * Vulkan Tutorial Common Library
 *
 * A few helpers to facilitate SDL usage à la c++11 style
 *
 *
 * SPDX-FileCopyrightText: 2022 Edouard Gomez
 * SPDX-License-Identifier: MIT
 * ----------------------------------------------------------------------- */

#pragma once

#include <SDL2/SDL.h>

#include <memory>

namespace egomez
{
namespace vulkan_tutorial
{

/** SDL tag type to be able to close the library w/ a functor */
struct SDL_LibraryTag
{
};

/** Functor that destroys known SDL types
 * - SDL itself (via the SDL_LibraryTag tag type)
 * - SDL_Window
 * - SDL_Renderer
 */
struct SDLDeleter
{
    /** SDL library closer */
    void operator()(SDL_LibraryTag* l)
    {
        if (l)
        {
            SDL_Quit();
        }
    }

    /** SDL_Window destroyer */
    void operator()(SDL_Window* w)
    {
        if (w)
        {
            SDL_DestroyWindow(w);
        }
    }

    /** SDL_Renderer destroyer */
    void operator()(SDL_Renderer* r)
    {
        if (r)
        {
            SDL_DestroyRenderer(r);
        }
    }
};

using unique_sdl_library = std::unique_ptr<SDL_LibraryTag, SDLDeleter>;
using unique_sdl_window = std::unique_ptr<SDL_Window, SDLDeleter>;
using unique_sdl_renderer = std::unique_ptr<SDL_Renderer, SDLDeleter>;

} // namespace vulkan_tutorial
} // namespace egomez
