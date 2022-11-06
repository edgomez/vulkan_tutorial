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

#include <egomez/vulkan_tutorial/sdl_helpers.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_vulkan.h>

#include <utility>
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <cstdio>
#include <cstdlib>
#include <exception>
#include <string>

namespace
{
using namespace egomez::vulkan_tutorial;

static const std::string s_app_name{"VulkanTurial01"};
static const std::string s_window_title{"Vulkan Tutorial 01 - Getting an instance"};
static const int s_window_width = 640;
static const int s_window_height = 480;

class ApplicationError : public std::runtime_error
{
  public:
    ApplicationError(const char* what) : std::runtime_error(what)
    {
    }
};

class VulkanApplication
{
  public:
    VulkanApplication(const std::string& app_name, const std::string& window_title, int window_width, int window_height)
        : m_app_name(app_name), m_window_title(window_title), m_window_width(window_width),
          m_window_height(window_height)
    {
    }

    int run()
    {
        initSDL();
        initVulkan();

        bool must_quit = false;
        while (!must_quit)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_QUIT:
                    must_quit = true;
                    break;
                default:
                    break;
                }
            }

            // XXX: render something
            // XXX: swap
        }

        return EXIT_SUCCESS;
    }

  protected:
    void initSDL()
    {
        createSDLWindow();
        getRequiredExtensionsFromSDL();
    }

    void initVulkan()
    {
        createVulkanInstance();
    }

    void createSDLWindow()
    {
        int res = SDL_Init(SDL_INIT_EVERYTHING);
        if (res)
        {
            throw ApplicationError("failed initalizing the SDL library");
        }
        m_library = unique_sdl_library{reinterpret_cast<SDL_LibraryTag*>(1)};

        m_window =
            unique_sdl_window{SDL_CreateWindow(m_window_title.c_str(), SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                               m_window_width, m_window_height, SDL_WINDOW_SHOWN | SDL_WINDOW_VULKAN)};
        if (!m_window)
        {
            throw ApplicationError("failed creating a SDL window");
        }

        if (!SDL_VIDEO_VULKAN)
        {
            throw ApplicationError("no vulkan support in your SDL library");
        }
    }

    void getRequiredExtensionsFromSDL()
    {
        unsigned int required_extensions_nb = 0;

        if (SDL_FALSE == SDL_Vulkan_GetInstanceExtensions(m_window.get(), &required_extensions_nb, nullptr))
        {
            throw ApplicationError(SDL_GetError());
        }

        if (required_extensions_nb > 0)
        {
            std::size_t cur_size = m_required_extensions.size();
            m_required_extensions.resize(cur_size + std::size_t(required_extensions_nb));
            if (SDL_FALSE == SDL_Vulkan_GetInstanceExtensions(m_window.get(), &required_extensions_nb,
                                                              m_required_extensions.data() + cur_size))
            {
                throw ApplicationError("failed getting the required vulkan extensions list from SDL");
            }

            for (auto& ext : m_required_extensions)
            {
                std::printf("info: required instance extension %s\n", ext);
            }
        }
    }

    void createVulkanInstance()
    {
        vk::ApplicationInfo app_info(m_app_name.c_str());
        app_info.apiVersion = VK_API_VERSION_1_0;

        vk::InstanceCreateInfo instance_info({}, &app_info, {}, {m_required_extensions});

        m_instance = vk::raii::Instance{m_context, instance_info};
    }

  private:
    /** Applicaiton name */
    const std::string m_app_name;

    /** Window title */
    const std::string m_window_title;

    /** Window width */
    int m_window_width;

    /** Window height */
    int m_window_height;

    /** RAII handling proper closing of the SDL library */
    unique_sdl_library m_library{nullptr};

    /** RAII handling proper destruction of the SDL window */
    unique_sdl_window m_window{nullptr};

    /** List of vulkan instance extensions required */
    std::vector<const char*> m_required_extensions;

    /** Context to which is attached all RAII Vulkan objects */
    vk::raii::Context m_context;

    /** the vulkan instance used throughout the tutorial code */
    vk::raii::Instance m_instance{nullptr};
};

} // namespace

extern "C" int main(int /* argc */, char** /* argv */)
{
    int res = EXIT_FAILURE;
    try
    {
        VulkanApplication app01{s_app_name, s_window_title, s_window_width, s_window_height};
        res = app01.run();
    }
    catch (ApplicationError& e)
    {
        std::printf("application error: %s\n", e.what());
    }
    catch (vk::SystemError& e)
    {
        std::printf("vulkan error: %s (%d)", e.what(), e.code().value());
    }
    catch (...)
    {
        std::printf("unexpected error: received unhandled exception\n");
    }

    return res;
}
