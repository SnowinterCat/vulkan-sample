// #include <ranges>

#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC true
#define VULKAN_HPP_NO_EXCEPTIONS
#define VULKAN_HPP_RAII_NO_EXCEPTIONS

#include <vulkan/vulkan_raii.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <spdlog/spdlog.h>

namespace fmtlib
{
#if __cpp_lib_format >= 202311L
    using namespace std;
#else
    #include <fmt/xchar.h>
    using namespace fmt;
#endif
} // namespace fmtlib

namespace outptrlib
{
#if defined(__cpp_lib_out_ptr)
    using namespace std;
#else
    #include <ztd/out_ptr.hpp>
    using namespace ztd::out_ptr;
#endif
} // namespace outptrlib

namespace expectedlib
{
#if defined(__cpp_lib_expected)
    using namespace std;
#else
    #include <tl/expected.hpp>
    using namespace tl;
#endif
} // namespace expectedlib

namespace sdl3
{
    struct Quiter {
        void operator()(void *ptr) { ptr != nullptr ? SDL_Quit() : void(); }
    };
    struct WindowDeleter {
        void operator()(SDL_Window *ptr) { ptr != nullptr ? SDL_DestroyWindow(ptr) : void(); }
    };

    namespace raii
    {

        template <std::invocable Func>
        class [[nodiscard]] ScopeExit {
        public:
            ScopeExit(Func func) : _func(std::move(func)) {}
            ~ScopeExit() { _func(); }

            ScopeExit(const ScopeExit &)            = delete;
            ScopeExit &operator=(const ScopeExit &) = delete;

            ScopeExit(ScopeExit &&rhs) noexcept : _func(std::exchange(rhs._func, {})) {}
            ScopeExit &operator=(ScopeExit &&rhs) noexcept
            {
                if (this != &rhs) {
                    std::swap(_func, rhs._func);
                }
                return *this;
            }

        private:
            Func _func;
        };

        using Init   = std::unique_ptr<void, Quiter>;
        using Window = std::unique_ptr<SDL_Window, WindowDeleter>;
    } // namespace raii

    auto getVulkanInstanceExtensions() -> std::span<const char *const>;
    auto getVulkanSurfaceOfWindow(SDL_Window *window, const ::vk::raii::Instance &instance)
        -> std::tuple<::vk::Result, vk::raii::SurfaceKHR>;

} // namespace sdl3

namespace vulkan
{
    constexpr bool EnableValidation = true;

    VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
        VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData, void * /*pUserData*/);

    bool hasLayer(std::span<::vk::LayerProperties> properties, const char *layer);
    bool hasExtension(std::span<::vk::ExtensionProperties> properties, const char *extension);

    bool hasLayer(std::span<const char *const> layers, const char *layer);
    bool hasExtension(std::span<const char *const> extensions, const char *extension);

    auto selectDeviceQueueFamilyIndex(std::span<const ::vk::QueueFamilyProperties> properties,
                                      ::vk::QueueFlags required, ::vk::QueueFlags refused)
        -> uint64_t;
    auto selectPresentQueueFamilyIndex(const ::vk::raii::PhysicalDevice            &physicalDevice,
                                       const ::vk::raii::SurfaceKHR                &surface,
                                       std::span<const ::vk::QueueFamilyProperties> properties)
        -> uint64_t;
    auto selectDeviceQueueIndex(std::span<const ::vk::QueueFamilyProperties> properties,
                                const uint64_t &graphicsQueueFamilyIndex,
                                const uint64_t &transferQueueFamilyIndex,
                                const uint64_t &presentQueueFamilyIndex,
                                const uint64_t &computeQueueFamilyIndex)
        -> std::tuple<uint64_t, uint64_t, uint64_t, uint64_t>;

    auto makeDeviceQueueCreateInfos(const uint64_t &graphicsQueueIndex,
                                    const uint64_t &transferQueueIndex,
                                    const uint64_t &presentQueueIndex,
                                    const uint64_t &computeQueueIndex, //
                                    float graphicsProirity, float transferProirity,
                                    float presentProirity, float computeProirity)
        -> std::tuple<std::vector<float>, std::vector<::vk::DeviceQueueCreateInfo>>;

    auto createInstanceAndDebugUtilsMessenger(const ::vk::raii::Context   &context,
                                              std::span<const char *const> extensions)
        -> std::tuple<::vk::Result, ::vk::raii::Instance, ::vk::raii::DebugUtilsMessengerEXT>;
    auto getPhysicalDeviceGroup(const ::vk::raii::Instance &instance)
        -> std::tuple<::vk::Result, ::vk::PhysicalDeviceGroupProperties>;
    auto createDeviceAndQueueIndices(const ::vk::raii::Instance              &instance,
                                     const vk::PhysicalDeviceGroupProperties &properties,
                                     const ::vk::raii::SurfaceKHR            &surface,
                                     const ::vk::PhysicalDeviceFeatures      &features)
        -> std::tuple<::vk::Result, ::vk::raii::Device, uint64_t, uint64_t, uint64_t, uint64_t>;
    auto getDeviceQueue(const ::vk::raii::Device &device, uint32_t familyIndex, uint32_t index)
        -> std::tuple<::vk::Result, ::vk::raii::Queue>;
    auto getDeviceQueues(const ::vk::raii::Device &device, //
                         uint64_t graphicsQueueIndex, uint64_t transferQueueIndex,
                         uint64_t presentQueueIndex, uint64_t computeQueueIndex)
        -> std::tuple<::vk::Result, ::vk::raii::Queue, ::vk::raii::Queue, ::vk::raii::Queue,
                      ::vk::raii::Queue>;
    auto createSwapchain(const ::vk::raii::Device &device)
        -> std::tuple<::vk::Result, ::vk::raii::SwapchainKHR>;

} // namespace vulkan

#if defined(_WIN32)
int wmain([[maybe_unused]] int argc, [[maybe_unused]] const wchar_t *const *wargv)
#else
int main([[maybe_unused]] int argc, [[maybe_unused]] const char *const *argv)
#endif
{
    std::setlocale(LC_ALL, ".UTF-8");
    // under instance
    auto res       = vk::Result::eSuccess;
    auto context   = vk::raii::Context();
    auto instance  = vk::raii::Instance(nullptr);
    auto messenger = vk::raii::DebugUtilsMessengerEXT(nullptr);
    auto surface   = vk::raii::SurfaceKHR(nullptr);
    // under devive
    auto physicalDeviceGroup = vk::PhysicalDeviceGroupProperties();
    auto device              = vk::raii::Device(nullptr);
    auto graphicsQueue       = vk::raii::Queue(nullptr);
    auto transferQueue       = vk::raii::Queue(nullptr);
    auto presentQueue        = vk::raii::Queue(nullptr);
    auto computeQueue        = vk::raii::Queue(nullptr);
    auto graphicsQueueIndex  = uint64_t(0);
    auto transferQueueIndex  = uint64_t(0);
    auto presentQueueIndex   = uint64_t(0);
    auto computeQueueIndex   = uint64_t(0);
    auto swapchain           = vk::raii::SwapchainKHR(nullptr);
    // sdl3
    auto initer = sdl3::raii::ScopeExit(
        std::bind(&SDL_QuitSubSystem, SDL_InitSubSystem(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD)
                                          ? SDL_INIT_VIDEO | SDL_INIT_GAMEPAD
                                          : NULL));
    auto window = sdl3::raii::Window(
        SDL_CreateWindow("ImGui SDL3+Vulkan example", 1280, 720,
                         SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY));

    // instance
    if (std::tie(res, instance, messenger) = vulkan::createInstanceAndDebugUtilsMessenger(
            context, sdl3::getVulkanInstanceExtensions());
        res != vk::Result::eSuccess) {
        SPDLOG_ERROR("[Vulkan] Create Instance Error, code: {}, info: {}", static_cast<int>(res),
                     vk::to_string(res));
        return static_cast<int>(res);
    }
    // surface
    if (std::tie(res, surface) = sdl3::getVulkanSurfaceOfWindow(window.get(), instance);
        res != vk::Result::eSuccess) {
        SPDLOG_ERROR("[libsdl3] Create SufaceKHR of Window Error, info: {}", SDL_GetError());
        return static_cast<int>(res);
    }
    // physical device group
    if (std::tie(res, physicalDeviceGroup) = vulkan::getPhysicalDeviceGroup(instance);
        res != vk::Result::eSuccess) {
        SPDLOG_ERROR("[Vulkan] Find PhysicalDeviceGroup Error, code: {}, info: {}",
                     static_cast<int>(res), vk::to_string(res));
        return static_cast<int>(res);
    }
    // device
    if (std::tie(res, device, graphicsQueueIndex, transferQueueIndex, presentQueueIndex,
                 computeQueueIndex) =
            vulkan::createDeviceAndQueueIndices(instance, physicalDeviceGroup, surface, {});
        res != vk::Result::eSuccess) {
        SPDLOG_ERROR("[Vulkan] Create Device and QueueIndices Error, code: {}, info: {}",
                     static_cast<int>(res), vk::to_string(res));
        return static_cast<int>(res);
    }
    // device queues
    if (std::tie(res, graphicsQueue, transferQueue, presentQueue, computeQueue) =
            vulkan::getDeviceQueues(device, graphicsQueueIndex, transferQueueIndex,
                                    presentQueueIndex, computeQueueIndex);
        res != vk::Result::eSuccess) {
        SPDLOG_ERROR("[Vulkan] get DeviceQueues Error, code: {}, info: {}", static_cast<int>(res),
                     vk::to_string(res));
        return static_cast<int>(res);
    }
    // swapchain
    // if (std::tie(res, swapchain) = vulkan::createSwapchain(device); res != vk::Result::eSuccess)
    // {
    //     SPDLOG_ERROR("[Vulkan] Create Swapchain Error, code: {}, info: {}",
    //     static_cast<int>(res),
    //                  vk::to_string(res));
    //     return static_cast<int>(res);
    // }

    auto event = SDL_Event();
    auto run   = true;
    while (run) {
        while (SDL_PollEvent(&event)) {
            switch (event.type) {
            case SDL_EVENT_QUIT:
                run = false;
                SPDLOG_INFO("Get Message: SDL_EVENT_QUIT");
                break;
            case SDL_EVENT_WINDOW_CLOSE_REQUESTED:
                if (event.window.windowID == SDL_GetWindowID(window.get())) {
                    run = false;
                }
                SPDLOG_INFO("Get Message: SDL_EVENT_WINDOW_CLOSE_REQUESTED");
                break;
            default:
                SPDLOG_INFO("Get Message: {}", event.type);
                break;
            }
        }
        // SPDLOG_INFO("Render");
    }
    return 0;
}

namespace sdl3
{
    auto getVulkanInstanceExtensions() -> std::span<const char *const>
    {
        auto        extenCnt   = uint32_t();
        const auto *extensions = SDL_Vulkan_GetInstanceExtensions(&extenCnt);
        return std::span(extensions, extenCnt);
    }

    auto getVulkanSurfaceOfWindow(SDL_Window *window, const ::vk::raii::Instance &instance)
        -> std::tuple<::vk::Result, vk::raii::SurfaceKHR>
    {
        auto         res     = ::vk::Result::eSuccess;
        VkSurfaceKHR surface = nullptr;
        if (!SDL_Vulkan_CreateSurface(window, *instance, nullptr, &surface)) {
            res = ::vk::Result::eErrorSurfaceLostKHR;
        }
        return std::tuple(res, ::vk::raii::SurfaceKHR(instance, surface));
    }
} // namespace sdl3

namespace vulkan
{
    VKAPI_ATTR VkBool32 VKAPI_CALL defaultDebugMessengerCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
        VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
        VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData, void * /*pUserData*/)
    {
        if (static_cast<uint32_t>(pCallbackData->messageIdNumber) == 0x822806fa) {
            // Validation Warning: vkCreateInstance(): to enable extension VK_EXT_debug_utils,
            // but this extension is intended to support use by applications when
            // debugging and it is strongly recommended that it be otherwise avoided.
            return ::vk::False;
        }
        if (static_cast<uint32_t>(pCallbackData->messageIdNumber) == 0xe8d1a9fe) {
            // Validation Performance Warning: Using debug builds of the validation layers *will*
            // adversely affect performance.
            return ::vk::False;
        }
        if (static_cast<::vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity) ==
                ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo &&
            strcmp(pCallbackData->pMessageIdName, "WARNING-DEBUG-PRINTF") != 0) {
            // 忽略 info 等级中，除了 vk::ValidationFeatureEnableEXT::eDebugPrintf 的所有消息
            return ::vk::False;
        }

        auto tmp = fmtlib::format(
            "\n{}: {}:\n\tmessageIDName   = <{}>\n\tmessageIdNumber = {}\n\tmessage         = <{}>",
            ::vk::to_string(
                static_cast<::vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)),
            ::vk::to_string(static_cast<::vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)),
            pCallbackData->pMessageIdName, pCallbackData->messageIdNumber, pCallbackData->pMessage);
        if (0 < pCallbackData->queueLabelCount) {
            tmp = fmtlib::format("{}\n\tQueue Labels:", tmp);
            for (uint32_t i = 0; i < pCallbackData->queueLabelCount; ++i) {
                tmp = fmtlib::format("{}\n\t\tlabelName = <{}>", tmp,
                                     pCallbackData->pQueueLabels[i].pLabelName);
            }
        }
        if (0 < pCallbackData->cmdBufLabelCount) {
            tmp = fmtlib::format("{}\n\tCommandBuffer Labels:", tmp);
            for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
                tmp = fmtlib::format("{}\n\t\tlabelName = <{}>", tmp,
                                     pCallbackData->pCmdBufLabels[i].pLabelName);
            }
        }
        if (0 < pCallbackData->objectCount) {
            tmp = fmtlib::format("{}\n\tObjects:", tmp);
            for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
                tmp = fmtlib::format(
                    "{}\n\t\tObject {}\n\t\t\tobjectType   = {}\n\t\t\tobjectHandle = {}", tmp, i,
                    ::vk::to_string(
                        static_cast<::vk::ObjectType>(pCallbackData->pObjects[i].objectType)),
                    pCallbackData->pObjects[i].objectHandle);
                if (pCallbackData->pObjects[i].pObjectName != nullptr) {
                    tmp = fmtlib::format("{}\n\t\t\tobjectName   = <{}>", tmp,
                                         pCallbackData->pObjects[i].pObjectName);
                }
            }
        }

        auto logLevel = spdlog::level::level_enum();
        switch (static_cast<::vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) {
        case ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
            logLevel = spdlog::level::debug;
            break;
        case ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
            logLevel = spdlog::level::info;
            break;
        case ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
            logLevel = spdlog::level::warn;
            break;
        case ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
            logLevel = spdlog::level::err;
            break;
        }
        SPDLOG_LOGGER_CALL(spdlog::default_logger_raw(), logLevel, "{}", tmp);
        return ::vk::False;
    }

    bool hasLayer(std::span<::vk::LayerProperties> properties, const char *layer)
    {
        return std::any_of(properties.begin(), properties.end(), [&layer](const auto &property) {
            return strcmp(property.layerName, layer) == 0;
        });
    }
    bool hasExtension(std::span<::vk::ExtensionProperties> properties, const char *extension)
    {
        return std::any_of(properties.begin(), properties.end(), [&extension](const auto &pp) {
            return strcmp(pp.extensionName, extension) == 0;
        });
    }

    auto selectDeviceQueueFamilyIndex(std::span<const ::vk::QueueFamilyProperties> properties,
                                      ::vk::QueueFlags required, ::vk::QueueFlags refused)
        -> uint64_t
    {
        for (size_t i = 0; i < properties.size(); ++i) {
            if (properties[i].queueFlags & required && !(properties[i].queueFlags & refused)) {
                return static_cast<uint64_t>(i);
            }
        }
        return std::numeric_limits<uint64_t>::max();
    }

    auto selectPresentQueueFamilyIndex(const ::vk::raii::PhysicalDevice            &physicalDevice,
                                       const ::vk::raii::SurfaceKHR                &surface,
                                       std::span<const ::vk::QueueFamilyProperties> properties)
        -> uint64_t
    {
        for (size_t i = 0; i < properties.size(); ++i) {
            if (physicalDevice.getSurfaceSupportKHR(i, surface) == ::vk::True) {
                return i;
            }
        }
        return std::numeric_limits<uint64_t>::max();
    }

    auto selectDeviceQueueIndex(std::span<const ::vk::QueueFamilyProperties> properties,
                                const uint64_t &graphicsQueueFamilyIndex,
                                const uint64_t &transferQueueFamilyIndex,
                                const uint64_t &presentQueueFamilyIndex,
                                const uint64_t &computeQueueFamilyIndex)
        -> std::tuple<uint64_t, uint64_t, uint64_t, uint64_t>
    {
        uint64_t              graphicsQueueIndex = std::numeric_limits<uint64_t>::max();
        uint64_t              transferQueueIndex = std::numeric_limits<uint64_t>::max();
        uint64_t              presentQueueIndex  = std::numeric_limits<uint64_t>::max();
        uint64_t              computeQueueIndex  = std::numeric_limits<uint64_t>::max();
        std::vector<uint32_t> remainQueueCounts(properties.size());
        for (size_t i = 0; i < properties.size(); ++i) {
            remainQueueCounts[i] = properties[i].queueCount;
        }

        auto calcIndex = [](size_t maxSize, uint32_t rawCount, uint32_t &remainCount,
                            uint64_t familyIndex) -> uint64_t {
            uint64_t index = std::numeric_limits<uint64_t>::max();
            if (familyIndex < maxSize) {
                index = static_cast<uint64_t>(rawCount - remainCount) << 32 | familyIndex;
                remainCount ? --remainCount : 0;
            }
            return index;
        };
        graphicsQueueIndex =
            calcIndex(properties.size(), properties[graphicsQueueFamilyIndex].queueCount,
                      remainQueueCounts[graphicsQueueFamilyIndex], graphicsQueueFamilyIndex);
        computeQueueIndex =
            calcIndex(properties.size(), properties[computeQueueFamilyIndex].queueCount,
                      remainQueueCounts[computeQueueFamilyIndex], computeQueueFamilyIndex);
        transferQueueIndex =
            calcIndex(properties.size(), properties[transferQueueFamilyIndex].queueCount,
                      remainQueueCounts[transferQueueFamilyIndex], transferQueueFamilyIndex);
        presentQueueIndex =
            calcIndex(properties.size(), properties[presentQueueFamilyIndex].queueCount,
                      remainQueueCounts[presentQueueFamilyIndex], presentQueueFamilyIndex);
        return std::tuple(graphicsQueueIndex, transferQueueIndex, presentQueueIndex,
                          computeQueueIndex);
    }

    auto makeDeviceQueueCreateInfos(const uint64_t &graphicsQueueIndex,
                                    const uint64_t &transferQueueIndex,
                                    const uint64_t &presentQueueIndex,
                                    const uint64_t &computeQueueIndex, //
                                    float graphicsProirity, float transferProirity,
                                    float presentProirity, float computeProirity)
        -> std::tuple<std::vector<float>, std::vector<::vk::DeviceQueueCreateInfo>>
    {
        auto priorities = std::vector<float>(4, 0.0F);
        auto infos      = std::vector<::vk::DeviceQueueCreateInfo>();
        // sort
        std::vector<std::tuple<uint64_t, uint64_t, float>> seq(4);
        seq[0] = std::tuple(graphicsQueueIndex & 0xFFFFFFFF, graphicsQueueIndex >> 32,
                            -graphicsProirity);
        seq[1] = std::tuple(transferQueueIndex & 0xFFFFFFFF, transferQueueIndex >> 32,
                            -transferProirity);
        seq[2] =
            std::tuple(presentQueueIndex & 0xFFFFFFFF, presentQueueIndex >> 32, -presentProirity);
        seq[3] =
            std::tuple(computeQueueIndex & 0xFFFFFFFF, computeQueueIndex >> 32, -computeProirity);
        std::sort(seq.begin(), seq.end());
        // make
        size_t last   = 0;
        priorities[0] = -std::get<2>(seq[0]);
        for (size_t i = 1; i < seq.size(); ++i) {
            const auto &[lastFamilyIndex, lastIndex, unused] = seq[i - 1];
            const auto &[familyIndex, index, priority]       = seq[i];

            priorities[i] = -priority;
            if (familyIndex == lastFamilyIndex) {
                continue;
            }
            infos.push_back(
                ::vk::DeviceQueueCreateInfo({}, lastFamilyIndex, lastIndex + 1, &priorities[last]));
            last = i;
        }
        const auto &[familyIndex, index, priority] = *seq.rbegin();
        infos.push_back(::vk::DeviceQueueCreateInfo({}, familyIndex, index + 1, &priorities[last]));
        for (size_t i = 0; i < infos.size(); ++i) {
            if (infos[i].queueFamilyIndex == 0xFFFFFFFF) {
                infos.resize(i);
                break;
            }
        }
        return std::tuple(std::move(priorities), std::move(infos));
    }

    auto createInstanceAndDebugUtilsMessenger(const ::vk::raii::Context   &context,
                                              std::span<const char *const> extensions)
        -> std::tuple<::vk::Result, ::vk::raii::Instance, ::vk::raii::DebugUtilsMessengerEXT>
    {
        auto res       = vk::Result::eSuccess;
        auto instance  = ::vk::raii::Instance(nullptr);
        auto messenger = ::vk::raii::DebugUtilsMessengerEXT(nullptr);

        auto layerProperties     = context.enumerateInstanceLayerProperties();
        auto extensionProperties = context.enumerateInstanceExtensionProperties();
        auto enabledLayers       = std::vector<const char *>();
        auto enabledExtensions   = std::vector<const char *>(extensions.begin(), extensions.end());

        auto appInfo = ::vk::ApplicationInfo("", ::vk::makeApiVersion(0, 0, 1, 0), "No Engine",
                                             ::vk::makeApiVersion(0, 0, 1, 0), ::vk::ApiVersion13);

        auto messengerInfo = ::vk::DebugUtilsMessengerCreateInfoEXT(
            {},
            ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo |
                ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                ::vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
            ::vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                ::vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
                ::vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
            &defaultDebugMessengerCallback, nullptr);
        auto vdFeatureEnable = {::vk::ValidationFeatureEnableEXT::eDebugPrintf};
        auto vdFeatures      = ::vk::ValidationFeaturesEXT(vdFeatureEnable, {}, &messengerInfo);
        if (EnableValidation) {
            if (hasLayer(layerProperties, "VK_LAYER_KHRONOS_validation")) {
                enabledLayers.push_back("VK_LAYER_KHRONOS_validation");
            }
            if (hasLayer(layerProperties, "VK_LAYER_LUNARG_monitor")) {
                enabledLayers.push_back("VK_LAYER_LUNARG_monitor");
            }
            if (hasExtension(extensionProperties, VK_EXT_DEBUG_UTILS_EXTENSION_NAME)) {
                enabledExtensions.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);
            }
        }

        // create ::vk::raii::instance
        if (res == ::vk::Result::eSuccess) {
            auto instanceInfo =
                ::vk::InstanceCreateInfo({}, &appInfo, enabledLayers, enabledExtensions,
                                         EnableValidation ? &vdFeatures : nullptr);
            auto val = context.createInstance(instanceInfo);
            val ? void(instance = std::move(val.value())) : void(res = val.error());
        }

        // create ::vk::raii::DebugUtilsMessengerEXT
        if (res == ::vk::Result::eSuccess && EnableValidation) {
            auto val = instance.createDebugUtilsMessengerEXT(messengerInfo);
            val ? void(messenger = std::move(val.value())) : void(res = val.error());
        }
        return std::tuple(res, std::move(instance), std::move(messenger));
    }

    auto getPhysicalDeviceGroup(const ::vk::raii::Instance &instance)
        -> std::tuple<::vk::Result, ::vk::PhysicalDeviceGroupProperties>
    {
        auto res          = ::vk::Result::eSuccess;
        auto deviceGroups = instance.enumeratePhysicalDeviceGroups();
        if (deviceGroups.empty()) {
            res = ::vk::Result::eErrorDeviceLost;
        }

        if (res == ::vk::Result::eSuccess) {
            std::vector<::vk::PhysicalDeviceType> physicalDeviceTypes;
            physicalDeviceTypes.reserve(deviceGroups.size());
            for (const auto &property : deviceGroups) {
                physicalDeviceTypes.push_back(property.physicalDevices[0]
                                                  .getProperties(*instance.getDispatcher())
                                                  .deviceType);
            }
            // select physical device group
            for (size_t i = 0; i < physicalDeviceTypes.size(); ++i) {
                if (physicalDeviceTypes[i] == ::vk::PhysicalDeviceType::eDiscreteGpu) {
                    return std::tuple(res, deviceGroups[i]);
                }
            }
            for (size_t i = 0; i < physicalDeviceTypes.size(); ++i) {
                if (physicalDeviceTypes[i] == ::vk::PhysicalDeviceType::eIntegratedGpu) {
                    return std::tuple(res, deviceGroups[i]);
                }
            }
            for (size_t i = 0; i < physicalDeviceTypes.size(); ++i) {
                if (physicalDeviceTypes[i] == ::vk::PhysicalDeviceType::eCpu) {
                    return std::tuple(res, deviceGroups[i]);
                }
            }
            for (size_t i = 0; i < physicalDeviceTypes.size(); ++i) {
                if (physicalDeviceTypes[i] == ::vk::PhysicalDeviceType::eVirtualGpu) {
                    return std::tuple(res, deviceGroups[i]);
                }
            }
            res = ::vk::Result::eErrorDeviceLost;
        }
        return std::tuple(res, ::vk::PhysicalDeviceGroupProperties());
    }

    auto createDeviceAndQueueIndices(const ::vk::raii::Instance              &instance,
                                     const vk::PhysicalDeviceGroupProperties &properties,
                                     const ::vk::raii::SurfaceKHR            &surface,
                                     const ::vk::PhysicalDeviceFeatures      &features)
        -> std::tuple<::vk::Result, ::vk::raii::Device, uint64_t, uint64_t, uint64_t, uint64_t>
    {
        auto     res                = ::vk::Result::eSuccess;
        auto     device             = ::vk::raii::Device(nullptr);
        uint64_t graphicsQueueIndex = 0;
        uint64_t transferQueueIndex = 0;
        uint64_t presentQueueIndex  = 0;
        uint64_t computeQueueIndex  = 0;

        auto physicalDevice = ::vk::raii::PhysicalDevice(instance, properties.physicalDevices[0]);
        // queue indices
        auto queueProperties = physicalDevice.getQueueFamilyProperties();
        graphicsQueueIndex   = selectDeviceQueueFamilyIndex(
            queueProperties, ::vk::QueueFlagBits::eGraphics, ::vk::QueueFlags());
        transferQueueIndex = selectDeviceQueueFamilyIndex(
            queueProperties, ::vk::QueueFlagBits::eTransfer, ::vk::QueueFlagBits::eGraphics);
        presentQueueIndex =
            physicalDevice.getSurfaceSupportKHR(graphicsQueueIndex, surface) == ::vk::True
                ? graphicsQueueIndex
                : selectPresentQueueFamilyIndex(physicalDevice, surface, queueProperties);
        computeQueueIndex = selectDeviceQueueFamilyIndex(
            queueProperties, ::vk::QueueFlagBits::eCompute, ::vk::QueueFlagBits::eGraphics);
        if (graphicsQueueIndex == std::numeric_limits<uint64_t>::max()) {
            res = ::vk::Result::eErrorDeviceLost;
        }
        if (transferQueueIndex == std::numeric_limits<uint64_t>::max()) {
            transferQueueIndex = graphicsQueueIndex;
        }
        if (computeQueueIndex == std::numeric_limits<uint64_t>::max()) {
            computeQueueIndex = graphicsQueueIndex;
        }
        std::tie(graphicsQueueIndex, transferQueueIndex, presentQueueIndex, computeQueueIndex) =
            selectDeviceQueueIndex(queueProperties, graphicsQueueIndex, transferQueueIndex,
                                   presentQueueIndex, computeQueueIndex);
        presentQueueIndex = std::numeric_limits<uint64_t>::max();
        auto [priorities, queueCreateInfos] =
            makeDeviceQueueCreateInfos(graphicsQueueIndex, transferQueueIndex, presentQueueIndex,
                                       computeQueueIndex, 1.0F, 1.0F, 0.5F, 0.5F);

        // layers and extensions
        // Device layers is deprecated!!!!!!!!!!
        // See: https://github.com/KhronosGroup/Vulkan-Loader/issues/1086
        // auto layerProperties = physicalDevice.enumerateDeviceLayerProperties();
        auto extenProperties = physicalDevice.enumerateDeviceExtensionProperties();
        auto enabledLayers   = std::vector<const char *>();
        auto enabledExtens   = std::vector<const char *>();
        if (hasExtension(extenProperties, VK_KHR_SWAPCHAIN_EXTENSION_NAME)) {
            enabledExtens.push_back(VK_KHR_SWAPCHAIN_EXTENSION_NAME);
        }

        // DeviceGroupDeviceCreateInfo
        auto devGroupInfo = ::vk::DeviceGroupDeviceCreateInfo(properties.physicalDeviceCount,
                                                              properties.physicalDevices);

        // create device
        {
            auto info = ::vk::DeviceCreateInfo({}, queueCreateInfos, enabledLayers, enabledExtens,
                                               &features, &devGroupInfo);
            auto val  = physicalDevice.createDevice(info);
            val ? void(device = std::move(val.value())) : void(res = val.error());
        }
        return std::tuple(res, std::move(device), graphicsQueueIndex, transferQueueIndex,
                          presentQueueIndex, computeQueueIndex);
    }

    auto getDeviceQueue(const ::vk::raii::Device &device, uint32_t familyIndex, uint32_t index)
        -> std::tuple<::vk::Result, ::vk::raii::Queue>
    {
        auto res   = ::vk::Result::eSuccess;
        auto queue = ::vk::raii::Queue(nullptr);
        if (familyIndex != std::numeric_limits<uint32_t>::max()) {
            auto val = device.getQueue(familyIndex, index);
            val ? void(queue = std::move(val.value())) : void(res = val.error());
        }
        return std::tuple(res, std::move(queue));
    }

    auto getDeviceQueues(const ::vk::raii::Device &device, //
                         uint64_t graphicsQueueIndex, uint64_t transferQueueIndex,
                         uint64_t presentQueueIndex, uint64_t computeQueueIndex)
        -> std::tuple<::vk::Result, ::vk::raii::Queue, ::vk::raii::Queue, ::vk::raii::Queue,
                      ::vk::raii::Queue>
    {
        auto res           = ::vk::Result::eSuccess;
        auto graphicsQueue = ::vk::raii::Queue(nullptr);
        auto transferQueue = ::vk::raii::Queue(nullptr);
        auto presentQueue  = ::vk::raii::Queue(nullptr);
        auto computeQueue  = ::vk::raii::Queue(nullptr);
        if (res == ::vk::Result::eSuccess) {
            std::tie(res, graphicsQueue) =
                getDeviceQueue(device, graphicsQueueIndex & 0xFFFFFFFF, graphicsQueueIndex >> 32);
        }
        if (res == ::vk::Result::eSuccess) {
            std::tie(res, transferQueue) =
                getDeviceQueue(device, transferQueueIndex & 0xFFFFFFFF, transferQueueIndex >> 32);
        }
        if (res == ::vk::Result::eSuccess) {
            std::tie(res, presentQueue) =
                getDeviceQueue(device, presentQueueIndex & 0xFFFFFFFF, presentQueueIndex >> 32);
        }
        if (res == ::vk::Result::eSuccess) {
            std::tie(res, computeQueue) =
                getDeviceQueue(device, computeQueueIndex & 0xFFFFFFFF, computeQueueIndex >> 32);
        }
        return std::tuple(res, graphicsQueue, transferQueue, presentQueue, computeQueue);
    }

    auto createSwapchain(const ::vk::raii::Device &device, const ::vk::raii::SurfaceKHR &surface)
        -> std::tuple<::vk::Result, ::vk::raii::SwapchainKHR>
    {
        auto res       = ::vk::Result::eSuccess;
        auto swapchain = ::vk::raii::SwapchainKHR(nullptr);

        {
            auto info = ::vk::SwapchainCreateInfoKHR({}, surface);
        }
        return std::tuple(res, std::move(swapchain));
    }
} // namespace vulkan
