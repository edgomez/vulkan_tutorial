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

#include <utility> // needed for std::exchange in vulkan_raii !
#include <vulkan/vulkan.hpp>
#include <vulkan/vulkan_raii.hpp>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <exception>
#include <string>

namespace
{
using namespace egomez::vulkan_tutorial;

static const std::string s_app_name{"VulkanTurial01"};
static const std::string s_window_title{"Vulkan Tutorial 01 - Getting an instance"};
static const int s_window_width = 640;
static const int s_window_height = 480;

static const char s_VK_EXT_debug_utils[] = "VK_EXT_debug_utils";
static const char s_VK_LAYER_KHRONOS_validation[] = "VK_LAYER_KHRONOS_validation";
static const char s_vkCreateDebugUtilsMessengerExt[] = "vkCreateDebugUtilsMessengerEXT";

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

    void setLoadValidationLayers(bool v)
    {
        m_load_validation_layers = v;
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
        pickupDevice();
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
        }
    }

    bool isLayerPresent(const char* layer_name)
    {
        const auto layerNamePredicate = [&layer_name](const vk::LayerProperties& a) -> bool {
            return !!strcmp(layer_name, a.layerName);
        };
        return std::end(m_layer_properties) !=
               std::find_if(std::begin(m_layer_properties), std::end(m_layer_properties), layerNamePredicate);
    }

    bool isExtensionPresent(const char* extension_name)
    {
        const auto extensionNamePredicate = [&extension_name](const vk::ExtensionProperties& a) -> bool {
            return !!strcmp(extension_name, a.extensionName);
        };
        return std::end(m_extension_properties) != std::find_if(std::begin(m_extension_properties),
                                                                std::end(m_extension_properties),
                                                                extensionNamePredicate);
    }

    static VKAPI_ATTR VkBool32 VKAPI_CALL debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT messageSeverity,
                                                        VkDebugUtilsMessageTypeFlagsEXT /* messageType */,
                                                        const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData,
                                                        void* /* pUserData */)
    {
        const char* severityCStr;
        switch (messageSeverity)
        {
        case int(vk::DebugUtilsMessageSeverityFlagBitsEXT::eError):
            severityCStr = "error";
            break;
        case int(vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo):
            severityCStr = "info";
            break;
        case int(vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose):
            severityCStr = "debug";
            break;
        case int(vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning):
            severityCStr = "warning";
            break;
        default:
            severityCStr = "unknown";
            break;
        }
        std::printf("%s: %s\n", severityCStr, pCallbackData->pMessage);
        return VK_FALSE;
    }

    void createVulkanInstance()
    {
        vk::ApplicationInfo app_info(m_app_name.c_str());
        app_info.apiVersion = VK_API_VERSION_1_0;

        m_layer_properties = vk::enumerateInstanceLayerProperties();
        m_extension_properties = vk::enumerateInstanceExtensionProperties();

#if 0
        for (auto& layer : m_layer_properties)
        {
            std::printf("info: available instance layer %s\n", layer.layerName.data());
        }
        for (const auto& extension : m_extension_properties)
        {
            std::printf("info: available instance extension %s\n", extension.extensionName.data());
        }
#endif // 0

        std::vector<const char*> layers_to_enable;
        bool hook_debug_print = false;

        if (m_load_validation_layers)
        {
            if (isLayerPresent(s_VK_LAYER_KHRONOS_validation))
            {
                std::printf("info: found required instance layer %s\n", s_VK_LAYER_KHRONOS_validation);
                layers_to_enable.push_back(s_VK_LAYER_KHRONOS_validation);
            }
            else
            {
                std::printf("warning: %s layer not found. will do without it.\n", s_VK_LAYER_KHRONOS_validation);
            }

            if (isExtensionPresent(s_VK_EXT_debug_utils))
            {
                m_required_extensions.push_back(s_VK_EXT_debug_utils);
                hook_debug_print = true;
                std::printf("info: found required instance extension %s\n", s_VK_EXT_debug_utils);
            }
            else
            {
                std::printf("warning: %s extension not found. will do without it.\n", s_VK_EXT_debug_utils);
            }
        }

        vk::InstanceCreateInfo instance_info({}, &app_info, {layers_to_enable}, {m_required_extensions});

        m_instance = vk::raii::Instance{m_context, instance_info};

        if (hook_debug_print)
        {
            vk::DebugUtilsMessengerCreateInfoEXT info(
                {},
                {vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError},
                {vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                 vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation},
                debugCallback, this);
            m_debug_utils_messenger = m_instance.createDebugUtilsMessengerEXT(info);
        }
    }

    void pickupDevice()
    {
        auto devices = m_instance.enumeratePhysicalDevices();
        for (const auto& d : devices)
        {
            auto dprops = d.getProperties();
            auto dfeats = d.getFeatures();
            if ((dprops.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
                 dprops.deviceType == vk::PhysicalDeviceType::eIntegratedGpu) &&
                dfeats.geometryShader)
            {
                std::printf("info: found device name=\"%s\" version=%d.%d.%d\n", dprops.deviceName.data(),
                            VK_API_VERSION_MAJOR(dprops.driverVersion), VK_API_VERSION_MINOR(dprops.driverVersion),
                            VK_API_VERSION_PATCH(dprops.driverVersion));
            }

            auto qprops = d.getQueueFamilyProperties();
            int i = 0;
            int graphicsQueue = -1;
            for (const auto& qp : qprops)
            {
                if (qp.queueFlags & vk::QueueFlagBits::eGraphics)
                {
                    graphicsQueue = i;
                    std::printf("info: found graphics queue index=%d\n", graphicsQueue);
                    break;
                }
                i++;
            }

            if (graphicsQueue >= 0)
            {
                // Found our device
                m_device = d;
                break;
            }
        }
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

    /** List of layers available */
    std::vector<vk::LayerProperties> m_layer_properties;

    /** List of extensions available */
    std::vector<vk::ExtensionProperties> m_extension_properties;

    /** Context to which is attached all RAII Vulkan objects */
    vk::raii::Context m_context;

    /** should load validation layers if available ? */
    bool m_load_validation_layers{false};

    /** the vulkan instance used throughout the tutorial code */
    vk::raii::Instance m_instance{nullptr};

    /** if debug was enabled and validation layers have been activated */
    vk::raii::DebugUtilsMessengerEXT m_debug_utils_messenger{nullptr};

    /** Physical device */
    vk::raii::PhysicalDevice m_device{nullptr};
};

} // namespace

extern "C" int main(int /* argc */, char** /* argv */)
{
    int res = EXIT_FAILURE;
    try
    {
        VulkanApplication app01{s_app_name, s_window_title, s_window_width, s_window_height};
#if !NDEBUG
        app01.setLoadValidationLayers(true);
#endif
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
