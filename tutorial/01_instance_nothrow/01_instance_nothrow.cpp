/* --------------------------------------------------------------------------
 * Vulkan Tutorial 01 (nothrow variant)
 *
 * Getting a vulkan instance and pick up a device
 *
 * This variant demonstrates how to use vulkan.hpp without exceptions
 * and without enhanced mode, using VULKAN_HPP_NO_EXCEPTIONS together
 * with VULKAN_HPP_DISABLE_ENHANCED_MODE.  vk::Result is checked
 * explicitly and memory for enumerations is managed via
 * ExplicitBuffer<T> (no dynamic standard-library containers).
 *
 * See:
 * https://vulkan-tutorial.com/Overview#page_Step-1-Instance-and-physical-device-selection
 *
 * SPDX-FileCopyrightText: 2022-2026 Edouard Gomez
 * SPDX-License-Identifier: MIT
 * ----------------------------------------------------------------------- */

#include <egomez/vulkan_tutorial/explicit_buffer.h>
#include <egomez/vulkan_tutorial/sdl_helpers.h>

#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>

/*
 * Disable vulkan.hpp exception-based error handling and enhanced mode.
 *
 * With VULKAN_HPP_NO_EXCEPTIONS:
 *   - functions return vk::Result directly (or via output parameters)
 *   - error handling is explicit via vk::Result checks
 *
 * With VULKAN_HPP_DISABLE_ENHANCED_MODE:
 *   - functions use the C-style calling convention (output pointers)
 *   - no implicit std::vector allocations in enumeration helpers
 *
 * We also silence the default VULKAN_HPP_ASSERT_ON_RESULT so that
 * failed results flow back to our code instead of aborting.
 */
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_ASSERT_ON_RESULT(expr)
#define VULKAN_HPP_DISABLE_ENHANCED_MODE

#include <vector> // vulkan.hpp may reference std::vector internally
#include <vulkan/vulkan.hpp>

#include <algorithm>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <iterator>
#include <string>

namespace
{
using namespace egomez::vulkan_tutorial;

static constexpr const char s_app_name[] = "VulkanTutorial01NoThrow";
static constexpr const char s_window_title[] = "Vulkan Tutorial 01 (nothrow) - Getting an instance";
static constexpr int        s_window_width = 640;
static constexpr int        s_window_height = 480;

static constexpr char s_VK_EXT_debug_utils[] = "VK_EXT_debug_utils";
static constexpr char s_VK_LAYER_KHRONOS_validation[] = "VK_LAYER_KHRONOS_validation";

/* ---------------------------------------------------------------------------
 * Two-phase Vulkan enumeration helper.
 *
 * Calls  enumerator(&count, nullptr)      to obtain the item count,
 * resizes @p buffer, then calls
 *        enumerator(&count, buffer.data()) to fill the buffer.
 *
 * The @p enumerator callable must have signature:
 *     vk::Result(uint32_t* count, T* data)
 * ----------------------------------------------------------------------- */
template <typename T, typename Enumerator> bool enumerateWithStorage(ExplicitBuffer<T>& buffer, Enumerator enumerator)
{
    uint32_t   count = 0;
    vk::Result result = enumerator(&count, nullptr);
    if (result != vk::Result::eSuccess)
    {
        std::printf("error: enumeration count failed (vk::Result %d)\n", static_cast<int>(result));
        return false;
    }
    if (count == 0)
    {
        buffer.clear();
        return true;
    }
    if (!buffer.resize(static_cast<std::size_t>(count)))
    {
        std::printf("error: allocation failed for %u items\n", count);
        return false;
    }
    result = enumerator(&count, buffer.data());
    if (result != vk::Result::eSuccess)
    {
        std::printf("error: enumeration fill failed (vk::Result %d)\n", static_cast<int>(result));
        return false;
    }
    return true;
}

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

    ~VulkanApplication()
    {
        cleanupVulkan();
    }

    /* Non-copyable, non-movable */
    VulkanApplication(const VulkanApplication&) = delete;
    VulkanApplication& operator=(const VulkanApplication&) = delete;

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
                    options.window_width = static_cast<int>(val);
                }
                i++;
            }
            else if (!strcmp(argv[i], "--height") && i < argc - 1)
            {
                char* end;
                long  val = std::strtol(argv[i + 1], &end, 10);
                if (end != argv[i + 1] && val > 0)
                {
                    options.window_height = static_cast<int>(val);
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
        if (!initSDL(options))
        {
            return EXIT_FAILURE;
        }

        if (!initVulkan(options))
        {
            return EXIT_FAILURE;
        }

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
    bool initSDL(const VulkanApplicationOptions& options)
    {
        if (!createSDLWindow(options))
        {
            return false;
        }
        if (!getRequiredExtensionsFromSDL())
        {
            return false;
        }
        return true;
    }

    bool initVulkan(const VulkanApplicationOptions& options)
    {
        if (!createVulkanInstance(options))
        {
            return false;
        }
        if (!pickupDevice(options.device_name))
        {
            return false;
        }
        return true;
    }

    bool createSDLWindow(const VulkanApplicationOptions& options)
    {
        bool success = SDL_Init(SDL_INIT_VIDEO | SDL_INIT_EVENTS);
        if (!success)
        {
            std::printf("error: failed initializing the SDL library\n");
            return false;
        }
        m_library = unique_sdl_library{reinterpret_cast<SDL_LibraryTag*>(1)};

        m_window = unique_sdl_window{
            SDL_CreateWindow(m_window_title.c_str(), options.window_width, options.window_height, SDL_WINDOW_VULKAN)};
        if (!m_window)
        {
            std::printf("error: failed creating a SDL window\n");
            return false;
        }
        SDL_SetWindowBordered(m_window.get(), true);

        m_renderer = unique_sdl_renderer{SDL_CreateRenderer(m_window.get(), "vulkan")};
        if (!m_renderer)
        {
            std::printf("error: failed creating a SDL renderer\n");
            return false;
        }

        return true;
    }

    bool getRequiredExtensionsFromSDL()
    {
        unsigned int required_extensions_nb = 0;

        const char* const* required_extensions = SDL_Vulkan_GetInstanceExtensions(&required_extensions_nb);
        if (!required_extensions)
        {
            std::printf("error: %s\n", SDL_GetError());
            return false;
        }

        for (unsigned int i = 0; i < required_extensions_nb; ++i)
        {
            if (!m_required_extensions.push_back(required_extensions[i]))
            {
                std::printf("error: allocation failed for required extensions\n");
                return false;
            }
        }

        return true;
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

    /* Debug callback --------------------------------------------------------
     *
     * The Vulkan loader dispatches through a C function pointer, so the
     * signature must use C types.  We static_cast to vk:: enums internally
     * for readability.
     * ---------------------------------------------------------------------- */
    static VkBool32 debugCallback(VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
                                  VkDebugUtilsMessageTypeFlagsEXT             messageType,
                                  const VkDebugUtilsMessengerCallbackDataEXT* pCallbackData, void* /*pUserData*/)
    {
        auto severity = static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity);
        auto msgType = static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageType);

        const char* severityCStr;
        switch (severity)
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
        if (msgType & vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral)
            messageTypeCStr = "general";
        else if (msgType & vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance)
            messageTypeCStr = "performance";
        else if (msgType & vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation)
            messageTypeCStr = "validation";
        else if (msgType & vk::DebugUtilsMessageTypeFlagBitsEXT::eDeviceAddressBinding)
            messageTypeCStr = "device address binding";

        std::printf("[%s] %s: %s\n", messageTypeCStr, severityCStr, pCallbackData->pMessage);
        return VK_FALSE;
    }

    bool createVulkanInstance(const VulkanApplicationOptions& options)
    {
        vk::ApplicationInfo app_info(m_app_name.c_str(), 0, nullptr, 0, VK_API_VERSION_1_0);

        // Enumerate available layers
        if (!enumerateWithStorage<vk::LayerProperties>(m_layer_properties,
                                                       [](uint32_t* count, vk::LayerProperties* data) {
                                                           return vk::enumerateInstanceLayerProperties(count, data);
                                                       }))
        {
            return false;
        }

        // Enumerate available extensions
        if (!enumerateWithStorage<vk::ExtensionProperties>(
                m_extension_properties, [](uint32_t* count, vk::ExtensionProperties* data) {
                    return vk::enumerateInstanceExtensionProperties(nullptr, count, data);
                }))
        {
            return false;
        }

#if 0
        for (std::size_t i = 0; i < m_layer_properties.size(); ++i)
        {
            std::printf("info: available instance layer %s\n", m_layer_properties[i].layerName.data());
        }
        for (std::size_t i = 0; i < m_extension_properties.size(); ++i)
        {
            std::printf("info: available instance extension %s\n", m_extension_properties[i].extensionName.data());
        }
#endif // 0

        ExplicitBuffer<const char*> layers_to_enable;
        bool                        hook_debug_print = false;

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

        vk::InstanceCreateInfo instance_info(
            {}, &app_info, static_cast<uint32_t>(layers_to_enable.size()), layers_to_enable.data(),
            static_cast<uint32_t>(m_required_extensions.size()), m_required_extensions.data());

        vk::Result result = vk::createInstance(&instance_info, nullptr, &m_instance);
        if (result != vk::Result::eSuccess)
        {
            std::printf("error: failed to create vulkan instance (vk::Result %d)\n", static_cast<int>(result));
            return false;
        }

        if (hook_debug_print)
        {
            /* Use a C struct for the debug messenger create-info because the
             * extension function pointer (PFN) expects C types and the
             * pfnUserCallback member type must match the C callback ABI. */
            VkDebugUtilsMessengerCreateInfoEXT messengerInfo = {};
            messengerInfo.sType = VK_STRUCTURE_TYPE_DEBUG_UTILS_MESSENGER_CREATE_INFO_EXT;
            messengerInfo.messageSeverity = static_cast<VkDebugUtilsMessageSeverityFlagsEXT>(
                vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning | vk::DebugUtilsMessageSeverityFlagBitsEXT::eError);
            messengerInfo.messageType = static_cast<VkDebugUtilsMessageTypeFlagsEXT>(
                vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral | vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation);
            messengerInfo.pfnUserCallback = &debugCallback;
            messengerInfo.pUserData = this;

            auto createFunc = reinterpret_cast<PFN_vkCreateDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(static_cast<VkInstance>(m_instance), "vkCreateDebugUtilsMessengerEXT"));
            if (createFunc)
            {
                VkResult vkr =
                    createFunc(static_cast<VkInstance>(m_instance), &messengerInfo, nullptr, &m_debug_utils_messenger);
                if (vkr != VK_SUCCESS)
                {
                    std::printf("warning: failed to create debug messenger (VkResult %d)\n", vkr);
                }
            }
        }

        return true;
    }

    struct DeviceEvaluation
    {
        vk::PhysicalDeviceProperties properties{};
        int                          score{0};
        int                          graphics_queue{-1};
    };

    DeviceEvaluation evaluateDevice(const vk::PhysicalDevice& device) const
    {
        DeviceEvaluation evaluation;

        device.getProperties(&evaluation.properties);

        vk::PhysicalDeviceFeatures dfeats;
        device.getFeatures(&dfeats);

        // Enumerate queue family properties (two-phase, void return)
        uint32_t queueCount = 0;
        device.getQueueFamilyProperties(&queueCount, nullptr);

        ExplicitBuffer<vk::QueueFamilyProperties> qprops;
        if (queueCount > 0)
        {
            if (!qprops.resize(static_cast<std::size_t>(queueCount)))
            {
                return evaluation; // allocation failure → unrateable
            }
            device.getQueueFamilyProperties(&queueCount, qprops.data());
        }

        // Check queue families for required graphics queue and optional capabilities
        bool hasGraphicsQueue = false;
        bool hasTransferQueue = false;
        bool hasComputeQueue = false;
        bool hasDedicatedTransferQueue = false;

        for (std::size_t i = 0; i < qprops.size(); ++i)
        {
            hasGraphicsQueue = hasGraphicsQueue || !!(qprops[i].queueFlags & vk::QueueFlagBits::eGraphics);
            hasTransferQueue = hasTransferQueue || !!(qprops[i].queueFlags & vk::QueueFlagBits::eTransfer);
            hasDedicatedTransferQueue =
                hasDedicatedTransferQueue || !!((qprops[i].queueFlags & vk::QueueFlagBits::eTransfer) &&
                                                !(qprops[i].queueFlags & vk::QueueFlagBits::eGraphics));
            hasComputeQueue = hasComputeQueue || !!(qprops[i].queueFlags & vk::QueueFlagBits::eCompute);

            if (evaluation.graphics_queue < 0 && (qprops[i].queueFlags & vk::QueueFlagBits::eGraphics))
            {
                evaluation.graphics_queue = static_cast<int>(i);
            }
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

    bool pickupDevice(const char* name)
    {
        ExplicitBuffer<vk::PhysicalDevice> devices;
        if (!enumerateWithStorage<vk::PhysicalDevice>(devices, [this](uint32_t* count, vk::PhysicalDevice* data) {
                return m_instance.enumeratePhysicalDevices(count, data);
            }))
        {
            std::printf("error: failed to enumerate physical devices\n");
            return false;
        }

        bool             has_best_device = false;
        std::size_t      best_device_index = 0;
        DeviceEvaluation best_evaluation{};

        for (std::size_t i = 0; i < devices.size(); ++i)
        {
            DeviceEvaluation                    evaluation = evaluateDevice(devices[i]);
            const vk::PhysicalDeviceProperties& dprops = evaluation.properties;

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
            const vk::PhysicalDevice&           d = devices[best_device_index];
            const vk::PhysicalDeviceProperties& dprops = best_evaluation.properties;

            if (dprops.deviceType == vk::PhysicalDeviceType::eDiscreteGpu ||
                dprops.deviceType == vk::PhysicalDeviceType::eIntegratedGpu)
            {
                std::printf("info: found device name=\"%s\" version=%d.%d.%d\n", dprops.deviceName.data(),
                            VK_API_VERSION_MAJOR(dprops.driverVersion), VK_API_VERSION_MINOR(dprops.driverVersion),
                            VK_API_VERSION_PATCH(dprops.driverVersion));
            }

            const int graphicsQueue = best_evaluation.graphics_queue;

            if (graphicsQueue >= 0)
            {
                std::printf("info: found graphics queue index=%d\n", graphicsQueue);
                // Found our device
                m_device = d;
                std::printf("info: using device %s\n", dprops.deviceName.data());
            }
        }
        if (!m_device)
        {
            std::printf("error: failed to find a suitable GPU\n");
            return false;
        }

        return true;
    }

    void cleanupVulkan()
    {
        if (m_debug_utils_messenger != VK_NULL_HANDLE)
        {
            auto destroyFunc = reinterpret_cast<PFN_vkDestroyDebugUtilsMessengerEXT>(
                vkGetInstanceProcAddr(static_cast<VkInstance>(m_instance), "vkDestroyDebugUtilsMessengerEXT"));
            if (destroyFunc)
            {
                destroyFunc(static_cast<VkInstance>(m_instance), m_debug_utils_messenger, nullptr);
            }
            m_debug_utils_messenger = VK_NULL_HANDLE;
        }

        if (m_instance)
        {
            m_instance.destroy(nullptr);
            m_instance = vk::Instance{};
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
    ExplicitBuffer<const char*> m_required_extensions;

    /** List of layers available */
    ExplicitBuffer<vk::LayerProperties> m_layer_properties;

    /** List of extensions available */
    ExplicitBuffer<vk::ExtensionProperties> m_extension_properties;

    /** should load validation layers if available ? */
    bool m_load_validation_layers{false};

    /** the vulkan instance used throughout the tutorial code */
    vk::Instance m_instance;

    /** if debug was enabled and validation layers have been activated */
    VkDebugUtilsMessengerEXT m_debug_utils_messenger{VK_NULL_HANDLE};

    /** Physical device (not owned — destroyed with instance) */
    vk::PhysicalDevice m_device;
};

} // namespace

int main(int argc, const char** argv)
{
    VulkanApplication        app01{s_app_name, s_window_title};
    VulkanApplicationOptions options;
    app01.parseCommandLine(argc, argv, options);
    return app01.run(options);
}
