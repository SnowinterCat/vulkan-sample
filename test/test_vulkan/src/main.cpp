// #include <print>

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
    using namespace fmt;
#endif
} // namespace fmtlib

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
        using Init   = std::unique_ptr<void, Quiter>;
        using Window = std::unique_ptr<SDL_Window, WindowDeleter>;
    } // namespace raii

    auto getVulkanInstanceExtensions() -> std::span<const char *const>;
    auto getVulkanSurfaceKHRofWindow(SDL_Window *window, ::vk::raii::Instance &instance)
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

    auto createInstanceAndDebugUtilsMessenger(const ::vk::raii::Context   &context,
                                              std::span<const char *const> extensions)
        -> std::tuple<::vk::Result, ::vk::raii::Instance, ::vk::raii::DebugUtilsMessengerEXT>;

} // namespace vulkan

#if defined(_WIN32)
int wmain([[maybe_unused]] int argc, [[maybe_unused]] const wchar_t *const *wargv)
#else
int main([[maybe_unused]] int argc, [[maybe_unused]] const char *const *argv)
#endif
{
    std::setlocale(LC_ALL, ".UTF-8");

    vk::Result                       res = vk::Result::eSuccess;
    vk::raii::Context                context;
    vk::raii::Instance               instance  = nullptr;
    vk::raii::DebugUtilsMessengerEXT messenger = nullptr;
    vk::raii::SurfaceKHR             surface   = nullptr;

    vk::PhysicalDeviceGroupProperties physicalDeviceGroup;
    vk::raii::Device                  device             = nullptr;
    vk::raii::Queue                   graphicsQueue      = nullptr;
    vk::raii::Queue                   transferQueue      = nullptr;
    vk::raii::Queue                   presentQueue       = nullptr;
    vk::raii::Queue                   computeQueue       = nullptr;
    uint64_t                          graphicsQueueIndex = 0;
    uint64_t                          transferQueueIndex = 0;
    uint64_t                          presentQueueIndex  = 0;
    uint64_t                          computeQueueIndex  = 0;

    sdl3::raii::Init   initer((void *)SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMEPAD));
    sdl3::raii::Window window(
        SDL_CreateWindow("ImGui SDL3+Vulkan example", 1280, 720,
                         SDL_WINDOW_VULKAN | SDL_WINDOW_RESIZABLE | SDL_WINDOW_HIGH_PIXEL_DENSITY));

    // instance
    if (std::tie(res, instance, messenger) = vulkan::createInstanceAndDebugUtilsMessenger(
            context, sdl3::getVulkanInstanceExtensions());
        res != vk::Result::eSuccess) {
        SPDLOG_ERROR("[Vulkan] Create Instance Error, code: {}, info: {}", static_cast<int>(res),
                     vk::to_string(res));
    }

    // surface
    if (std::tie(res, surface) = sdl3::getVulkanSurfaceKHRofWindow(window.get(), instance);
        res != vk::Result::eSuccess) {
        SPDLOG_ERROR("[libsdl3] Create SufaceKHR of Window Error, info: {}", SDL_GetError());
    }

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

    auto getVulkanSurfaceKHRofWindow(SDL_Window *window, ::vk::raii::Instance &instance)
        -> std::tuple<::vk::Result, vk::raii::SurfaceKHR>
    {
        ::vk::Result res = ::vk::Result::eSuccess;
        VkSurfaceKHR surface;
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

    auto createInstanceAndDebugUtilsMessenger(const ::vk::raii::Context   &context,
                                              std::span<const char *const> extensions)
        -> std::tuple<::vk::Result, ::vk::raii::Instance, ::vk::raii::DebugUtilsMessengerEXT>
    {
        ::vk::Result                       res       = vk::Result::eSuccess;
        ::vk::raii::Instance               instance  = nullptr;
        ::vk::raii::DebugUtilsMessengerEXT messenger = nullptr;

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
} // namespace vulkan
