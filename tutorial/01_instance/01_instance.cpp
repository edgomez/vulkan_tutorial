/* --------------------------------------------------------------------------
 * Vulkan Tutorial 01
 *
 * Getting a vulkan instance and pick up a device
 *
 * See:
 * https://vulkan-tutorial.com/Overview#page_Step-1-Instance-and-physical-device-selection
 *
 * SPDX-FileCopyrightText: 2022-2026 Edouard Gomez
 * SPDX-License-Identifier: MIT
 * ----------------------------------------------------------------------- */

#include <egomez/vulkan_tutorial/sdl_helpers.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

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

static constexpr const char s_app_name[] = "VulkanTutorial01";
static constexpr const char s_window_title[] = "Vulkan Tutorial 01 - Getting an instance";
static constexpr int        s_window_width = 640;
static constexpr int        s_window_height = 480;

static constexpr char s_VK_EXT_debug_utils[] = "VK_EXT_debug_utils";
static constexpr char s_VK_LAYER_KHRONOS_validation[] = "VK_LAYER_KHRONOS_validation";

[[maybe_unused]] static constexpr char s_vkCreateDebugUtilsMessengerExt[] = "vkCreateDebugUtilsMessengerEXT";

class ApplicationError : public std::runtime_error
{
  public:
    ApplicationError(const char* what) : std::runtime_error(what)
    {
    }
};

struct VulkanApplicationOptions
{
    bool        load_validation_layers{false};
    const char* device_name{nullptr};
    int         window_width{s_window_width};
    int         window_height{s_window_height};
};

class VulkanApplication
{
  public:
    VulkanApplication(const std::string& app_name, const std::string& window_title)
        : m_app_name(app_name), m_window_title(window_title)
    {
    }

    void parseCommandLine(int argc, const char** argv, VulkanApplicationOptions& options)
    {
        for (int i = 1; i < argc; i++)
        {
            if (!strcmp(argv[i], "--debug"))
            {
                options.load_validation_layers = true;
            }
            else if (!strcmp(argv[i], "--width") && i < argc - 1)
            {
                char* end;
                long  val = std::strtol(argv[i + 1], &end, 10);
                if (end != argv[i + 1] && val > 0)
                {
                    options.window_width = int(val);
                }
                i++;
            }
            else if (!strcmp(argv[i], "--height") && i < argc - 1)
            {
                char* end;
                long  val = std::strtol(argv[i + 1], &end, 10);
                if (end != argv[i + 1] && val > 0)
                {
                    options.window_height = int(val);
                }
                i++;
            }
            else if (!strcmp(argv[i], "--device") && i < argc - 1)
            {
                options.device_name = argv[i + 1];
                i++;
            }
        }
    }

    int run(const VulkanApplicationOptions& options)
    {
        initSDL(options);
        initVulkan(options);

        bool must_quit = false;
        while (!must_quit)
        {
            SDL_Event event;
            while (SDL_PollEvent(&event))
            {
                switch (event.type)
                {
                case SDL_EVENT_QUIT:
                    must_quit = true;
                    break;
                default:
                    break;
                }
            }

            SDL_RenderClear(m_renderer.get());
            SDL_RenderPresent(m_renderer.get());
        }

        return EXIT_SUCCESS;
    }

    void setLoadValidationLayers(bool v)
    {
        m_load_validation_layers = v;
    }

  protected:
    void initSDL(const VulkanApplicationOptions& options)
    {
        createSDLWindow(options);
        getRequiredExtensionsFromSDL();
    }

    void initVulkan(const VulkanApplicationOptions& options)
    {
        createVulkanInstance(options);
        pickupDevice(options.device_name);
    }

    void createSDLWindow(const VulkanApplicationOptions& options)
    {
        bool success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        if (!success)
        {
            throw ApplicationError("failed initializing the SDL library");
        }
        m_library = unique_sdl_library{reinterpret_cast<SDL_LibraryTag*>(1)};

        m_window = unique_sdl_window{
            SDL_CreateWindow(m_window_title.c_str(), options.window_width, options.window_height, SDL_WINDOW_VULKAN)};
        if (!m_window)
        {
            throw ApplicationError("failed creating a SDL window");
        }
        SDL_SetWindowBordered(m_window.get(), true);

        m_renderer = unique_sdl_renderer{SDL_CreateRenderer(m_window.get(), "vulkan")};
        if (!m_renderer)
        {
            throw ApplicationError("failed creating a SDL renderer");
        }
    }

    void getRequiredExtensionsFromSDL()
    {
        unsigned int required_extensions_nb = 0;

        const char* const* required_extensions = SDL_Vulkan_GetInstanceExtensions(&required_extensions_nb);
        if (!required_extensions)
        {
            throw ApplicationError(SDL_GetError());
        }

        if (required_extensions_nb > 0)
        {
            std::size_t cur_size = m_required_extensions.size();
            m_required_extensions.resize(cur_size + std::size_t(required_extensions_nb));
            std::memcpy(m_required_extensions.data() + cur_size, required_extensions,
                        required_extensions_nb * sizeof(const char*));
        }
    }

    bool isLayerPresent(const char* layer_name)
    {
        const auto layerNamePredicate = [&layer_name](const vk::LayerProperties& a) -> bool {
            return !strcmp(layer_name, a.layerName);
        };
        return std::end(m_layer_properties) !=
               std::find_if(std::begin(m_layer_properties), std::end(m_layer_properties), layerNamePredicate);
    }

    bool isExtensionPresent(const char* extension_name)
    {
        const auto extensionNamePredicate = [&extension_name](const vk::ExtensionProperties& a) -> bool {
            return !strcmp(extension_name, a.extensionName);
        };
        return std::end(m_extension_properties) != std::find_if(std::begin(m_extension_properties),
                                                                std::end(m_extension_properties),
                                                                extensionNamePredicate);
    }

    static VkBool32 debugCallback(vk::DebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                  vk::DebugUtilsMessageTypeFlagsEXT             messageType,
                                  const vk::DebugUtilsMessengerCallbackDataEXT* pCallbackData, void* /*pUserData*/)
    {
        const char* severityCStr;
        switch (messageSeverity)
        {
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            severityCStr = "error";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            severityCStr = "info";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            severityCStr = "debug";
            break;
        case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            severityCStr = "warning";
            break;
        default:
            severityCStr = "unknown";
            break;
        }

        const char* messageTypeCStr = "unknown";
        if (messageType & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
            messageTypeCStr = "general";
        else if (messageType & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
            messageTypeCStr = "performance";
        else if (messageType & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
            messageTypeCStr = "validation";
        else if (messageType & vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding)
            messageTypeCStr = "device address binding";

        std::printf("[%s] %s: %s\n", messageTypeCStr, severityCStr, pCallbackData->pMessage);
        return VK_FALSE;
    }

    void createVulkanInstance(const VulkanApplicationOptions& options)
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
        bool                     hook_debug_print = false;

        if (options.load_validation_layers)
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
            vk::DebugUtilsMessengerCreateInfoEXT info{};
            info.messageSeverity =
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;
            info.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                               vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                               vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation;
            info.pfnUserCallback = debugCallback;
            info.pUserData = this;
            m_debug_utils_messenger = m_instance.createDebugUtilsMessengerEXT(info);
        }
    }

    struct DeviceEvaluation
    {
        vk::PhysicalDeviceProperties properties;
        int                          score{0};
        int                          graphics_queue{-1};
    };

    DeviceEvaluation evaluateDevice(const vk::raii::PhysicalDevice& device)
    {
        DeviceEvaluation evaluation;

        evaluation.properties = device.getProperties();
        auto dfeats = device.getFeatures();

        // Check queue families for required graphics queue and optional capabilities
        auto qprops = device.getQueueFamilyProperties();
        bool hasGraphicsQueue = false;
        bool hasTransferQueue = false;
        bool hasComputeQueue = false;
        bool hasDedicatedTransferQueue = false;

        int queue_index = 0;
        for (const auto& qp : qprops)
        {
            hasGraphicsQueue = hasGraphicsQueue || !!(qp.queueFlags & vk::QueueFlagBits::eGraphics);
            hasTransferQueue = hasTransferQueue || !!(qp.queueFlags & vk::QueueFlagBits::eTransfer);
            hasDedicatedTransferQueue =
                hasDedicatedTransferQueue ||
                !!((qp.queueFlags & vk::QueueFlagBits::eTransfer) && !(qp.queueFlags & vk::QueueFlagBits::eGraphics));
            hasComputeQueue = hasComputeQueue || !!(qp.queueFlags & vk::QueueFlagBits::eCompute);

            if (evaluation.graphics_queue < 0 && (qp.queueFlags & vk::QueueFlagBits::eGraphics))
            {
                evaluation.graphics_queue = queue_index;
            }

            queue_index++;
        }

        // Device must have a graphics queue, otherwise it's unsuitable
        if (!hasGraphicsQueue)
        {
            return evaluation;
        }

        // Device type scoring: Discrete > Integrated > Virtual > CPU
        evaluation.score += 100 * (!!(evaluation.properties.deviceType == vk::PhysicalDeviceType::eCpu));
        evaluation.score += 1000 * (!!(evaluation.properties.deviceType == vk::PhysicalDeviceType::eVirtualGpu));
        evaluation.score += 10000 * (!!(evaluation.properties.deviceType == vk::PhysicalDeviceType::eIntegratedGpu));
        evaluation.score += 100000 * (!!(evaluation.properties.deviceType == vk::PhysicalDeviceType::eDiscreteGpu));

        // Larger texture support is better
        evaluation.score += static_cast<int>(evaluation.properties.limits.maxImageDimension2D);

        // Bonus for transfer queue support (better async data transfers)
        evaluation.score += 1000 * (!!hasTransferQueue);
        // Extra bonus for dedicated transfer queue
        evaluation.score += 2000 * (!!hasDedicatedTransferQueue);
        // Bonus for compute queue support (useful for post-processing, etc.)
        evaluation.score += 1000 * (!!hasComputeQueue);
        // Bonus for geometry shaders
        evaluation.score += 500 * (!!dfeats.geometryShader);
        // Bonus for multiViewport feature
        evaluation.score += 100 * (!!dfeats.multiViewport);

        return evaluation;
    }

    void pickupDevice(const char* name)
    {
        auto devices = m_instance.enumeratePhysicalDevices();

        bool             has_best_device = false;
        std::size_t      best_device_index = 0;
        DeviceEvaluation best_evaluation{};

        for (std::size_t i = 0; i < devices.size(); ++i)
        {
            DeviceEvaluation evaluation = evaluateDevice(devices[i]);
            const auto&      dprops = evaluation.properties;

            if (evaluation.score <= 0)
            {
                continue;
            }

            if (nullptr != name)
            {
                if (strcmp(name, dprops.deviceName.data()))
                {
                    continue;
                }
            }

            if (!has_best_device || evaluation.score > best_evaluation.score)
            {
                has_best_device = true;
                best_device_index = i;
                best_evaluation = evaluation;
            }
        }

        if (has_best_device)
        {
            const auto& d = devices[best_device_index];
            const auto& dprops = best_evaluation.properties;

            if (dprops.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
                dprops.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
            {
                std::printf("info: found device name=\"%s\" version=%d.%d.%d\n", dprops.deviceName.data(),
                            VK_API_VERSION_MAJOR(dprops.driverVersion), VK_API_VERSION_MINOR(dprops.driverVersion),
                            VK_API_VERSION_PATCH(dprops.driverVersion));
            }

            std::printf("info: found graphics queue index=%d\n", best_evaluation.graphics_queue);
            m_device = d;
            std::printf("info: using device %s\n", dprops.deviceName.data());
        }

        if (nullptr == m_device)
        {
            throw ApplicationError("failed to find a suitable GPU");
        }
    }

  private:
    /** Application name */
    const std::string m_app_name;

    /** Window title */
    const std::string m_window_title;

    /** RAII handling proper closing of the SDL library */
    unique_sdl_library m_library{nullptr};

    /** RAII handling proper destruction of the SDL window */
    unique_sdl_window m_window{nullptr};

    /** RAII handling proper destruction of the SDL renderer */
    unique_sdl_renderer m_renderer{nullptr};

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

int main(int argc, const char** argv)
{
    int res = EXIT_FAILURE;
    try
    {
        VulkanApplication        app01{s_app_name, s_window_title};
        VulkanApplicationOptions options;
        app01.parseCommandLine(argc, argv, options);
        res = app01.run(options);
    }
    catch (ApplicationError& e)
    {
        std::printf("application error: %s\n", e.what());
    }
    catch (vk::SystemError& e)
    {
        std::printf("vulkan error: %s (%d)\n", e.what(), e.code().value());
    }
    catch (...)
    {
        std::printf("unexpected error: received unhandled exception\n");
    }

    return res;
}
