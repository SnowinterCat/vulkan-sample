#include "vks/render/funcs.hpp"

// standard library
#include <bit>
#if __cplusplus > 202600L
#    include <format>
#    define VKS_FORMAT std
#else
#    include <fmt/xchar.h>
#    define VKS_FORMAT fmt
#endif
// third-party
#include <spdlog/spdlog.h>

VKS_BEGIN
VKS_RENDER_BEGIN

////////////////////////////////////////////////////////////////////////////////
/// default function
////////////////////////////////////////////////////////////////////////////////

VKAPI_ATTR VkBool32 VKAPI_CALL default_debug_messenger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData, void * /*pUserData*/)
{
    std::string outstr;
#ifndef NDEBUG
    if (static_cast<uint32_t>(pCallbackData->messageIdNumber) == 0x822806fa) {
        // Validation Warning: vkCreateInstance(): to enable extension VK_EXT_debug_utils, but this
        // extension is intended to support use by applications when debugging and it is strongly
        // recommended that it be otherwise avoided.
        return vk::False;
    }
    if (static_cast<uint32_t>(pCallbackData->messageIdNumber) == 0xe8d1a9fe) {
        // Validation Performance Warning: Using debug builds of the validation layers *will*
        // adversely affect performance.
        return vk::False;
    }
#endif
    outstr = VKS_FORMAT::format(
        "{}: {}:\n"
        "\tmessageIDName   = <{}>\n"
        "\tmessageIdNumber = <{}>\n"
        "\tmessage         = <{}>",
        vk::to_string(static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)),
        vk::to_string(static_cast<vk::DebugUtilsMessageTypeFlagsEXT>(messageTypes)),
        pCallbackData->pMessageIdName, pCallbackData->messageIdNumber, pCallbackData->pMessage);
    if (0 < pCallbackData->queueLabelCount) {
        outstr = VKS_FORMAT::format("{}\n\tQueue Labels:", outstr);
        for (uint32_t i = 0; i < pCallbackData->queueLabelCount; i++) {
            outstr = VKS_FORMAT::format("{}\n\t\tlabelName = <{}>", outstr,
                                        pCallbackData->pQueueLabels[i].pLabelName);
        }
    }
    if (0 < pCallbackData->cmdBufLabelCount) {
        outstr = VKS_FORMAT::format("{}\n\tCommandBuffer Labels:", outstr);
        for (uint32_t i = 0; i < pCallbackData->cmdBufLabelCount; i++) {
            outstr = VKS_FORMAT::format("{}\n\t\tlabelName = <{}>", outstr,
                                        pCallbackData->pCmdBufLabels[i].pLabelName);
        }
    }
    if (0 < pCallbackData->objectCount) {
        outstr = VKS_FORMAT::format("{}\n\tObjects:", outstr);
        for (uint32_t i = 0; i < pCallbackData->objectCount; i++) {
            outstr = VKS_FORMAT::format(
                "{}\n"
                "\t\tObject {}\n"
                "\t\t\tobjectType   = {}\n"
                "\t\t\tobjectHandle = {}",
                outstr, i,
                vk::to_string(static_cast<vk::ObjectType>(pCallbackData->pObjects[i].objectType)),
                pCallbackData->pObjects[i].objectHandle);
            if (pCallbackData->pObjects[i].pObjectName != nullptr) {
                outstr = VKS_FORMAT::format("{}\n\t\t\tobjectName   = <{}>", outstr,
                                            pCallbackData->pObjects[i].pObjectName);
            }
        }
    }
    switch (static_cast<vk::DebugUtilsMessageSeverityFlagBitsEXT>(messageSeverity)) {
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose:
        VKS_RENDER_LOG_TRACE("{}", outstr);
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eInfo:
        VKS_RENDER_LOG_INFO("{}", outstr);
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning:
        VKS_RENDER_LOG_WARN("{}", outstr);
        break;
    case vk::DebugUtilsMessageSeverityFlagBitsEXT::eError:
        VKS_RENDER_LOG_ERROR("{}", outstr);
        break;
    }
    return vk::False;
}

auto default_queue_create_info_inefficient(vk::PhysicalDevice     physicalDevice,     //
                                           vk::SurfaceKHR         surface,            //
                                           std::vector<float>    &queuePriorities,    //
                                           std::vector<uint32_t> &graphicsQueueInfos, //
                                           std::vector<uint32_t> &presentQueueInfos,  //
                                           std::vector<uint32_t> &computeQueueInfos,  //
                                           std::vector<uint32_t> &transferQueueInfos, //
                                           std::error_code       &errc                //
                                           ) -> std::vector<vk::DeviceQueueCreateInfo>
{
    auto info       = std::vector<vk::DeviceQueueCreateInfo>();
    auto outstr     = std::string();
    auto properties = physicalDevice.getQueueFamilyProperties();

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
    outstr =
        VKS_FORMAT::format("{} has queue families: {}",
                           physicalDevice.getProperties().deviceName.data(), properties.size());
    for (const auto &property : properties) {
        outstr = VKS_FORMAT::format(
            "{}\n\tcount: {}, flags: {}, time stamp: {}, min image transfer granularity: {}*{}*{}",
            outstr, property.queueCount, vk::to_string(property.queueFlags),
            property.timestampValidBits, property.minImageTransferGranularity.width,
            property.minImageTransferGranularity.height,
            property.minImageTransferGranularity.depth);
    }
    VKS_RENDER_LOG_TRACE("{}", outstr);
#endif

    constexpr uint32_t MaxQueueFlagWeight = std::popcount(static_cast<uint32_t>(
        VK_QUEUE_GRAPHICS_BIT | VK_QUEUE_COMPUTE_BIT | VK_QUEUE_TRANSFER_BIT |
        VK_QUEUE_SPARSE_BINDING_BIT | VK_QUEUE_PROTECTED_BIT | VK_QUEUE_VIDEO_DECODE_BIT_KHR |
        VK_QUEUE_VIDEO_ENCODE_BIT_KHR | VK_QUEUE_OPTICAL_FLOW_BIT_NV));
    std::vector<std::pair<uint32_t, uint32_t>> queueWeights(properties.size());
    for (size_t i = 0; i < properties.size(); ++i) {
        queueWeights[i] = std::pair(
            MaxQueueFlagWeight - std::popcount(static_cast<uint32_t>(properties[i].queueFlags)), i);
    }
    std::sort(queueWeights.begin(), queueWeights.end(), std::greater<>{});

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
    outstr = "queue family weights:";
    for (const auto &queueWeight : queueWeights) {
        outstr = VKS_FORMAT::format("{}\n\tqueue family index: {} has weight: {}", outstr,
                                    queueWeight.second, queueWeight.first);
    }
    VKS_RENDER_LOG_TRACE("{}", outstr);
#endif

    return info;
}

////////////////////////////////////////////////////////////////////////////////
/// function
////////////////////////////////////////////////////////////////////////////////

auto make_unique_surface(vk::Instance instance, vk::SurfaceKHR surface) -> vk::UniqueSurfaceKHR
{
    return vk::UniqueSurfaceKHR(surface, vk::ObjectDestroy<vk::Instance, vk::DispatchLoaderDynamic>(
                                             instance, nullptr, VULKAN_HPP_DEFAULT_DISPATCHER));
}

auto create_instance_and_debug_messenger(PFN_vkGetInstanceProcAddr            pfn1,              //
                                         PFN_vkDebugUtilsMessengerCallbackEXT pfn2,              //
                                         std::string_view                     appName,           //
                                         uint32_t                             appVersion,        //
                                         uint32_t                             wndExtensionCount, //
                                         const char *const                   *wndExtensionNames, //
                                         std::error_code                     &errc               //
                                         ) -> std::pair<vk::Instance, vk::DebugUtilsMessengerEXT>
{
    assert(pfn1 && "PFN_vkGetInstanceProcAddr pfn1 cannot be nullptr");

    vk::Result                 res;
    vk::Instance               instance;
    vk::DebugUtilsMessengerEXT messenger;
    VULKAN_HPP_DEFAULT_DISPATCHER.init(pfn1);

    auto layerProperties     = vk::enumerateInstanceLayerProperties().value;
    auto extensionProperties = vk::enumerateInstanceExtensionProperties().value;
    auto outstr              = std::string();

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
    outstr = "window required extensions:";
    for (uint32_t i = 0; i < wndExtensionCount; ++i) {
        outstr = VKS_FORMAT::format("{}\n\t{}", outstr, wndExtensionNames[i]);
    }
    VKS_RENDER_LOG_TRACE("{}", outstr);
    outstr = "vulkan instance layers:";
    for (const auto &property : layerProperties) {
        outstr = VKS_FORMAT::format(
            "{}\n\tname: {}, des: {}, impl version: {}.{}.{}.{}, spec version: {}.{}.{}.{}", outstr,
            property.layerName.data(), property.description.data(),
            vk::apiVersionMajor(property.implementationVersion),
            vk::apiVersionMinor(property.implementationVersion),
            vk::apiVersionPatch(property.implementationVersion),
            vk::apiVersionVariant(property.implementationVersion),
            vk::apiVersionMajor(property.specVersion), vk::apiVersionMinor(property.specVersion),
            vk::apiVersionPatch(property.specVersion), vk::apiVersionVariant(property.specVersion));
    }
    VKS_RENDER_LOG_TRACE("{}", outstr);
    outstr = "vulkan instance extensions:";
    for (const auto &property : extensionProperties) {
        outstr = VKS_FORMAT::format(
            "{}\n\tname: {}, spec version: {}.{}.{}.{}", outstr, property.extensionName.data(),
            vk::apiVersionMajor(property.specVersion), vk::apiVersionMinor(property.specVersion),
            vk::apiVersionPatch(property.specVersion), vk::apiVersionVariant(property.specVersion));
    }
    VKS_RENDER_LOG_TRACE("{}", outstr);
#endif
    // layers
    std::vector<const char *> layers;
    std::vector<const char *> requiredLayers;
    if (pfn2 != nullptr) {
        requiredLayers.push_back("VK_LAYER_KHRONOS_validation");
        requiredLayers.push_back("VK_LAYER_LUNARG_monitor");
    }
    for (const auto &reqLayer : requiredLayers) {
        if (std::all_of(layerProperties.begin(), layerProperties.end(),
                        [reqLayer](const vk::LayerProperties &property) {
                            return strcmp(reqLayer, property.layerName.data()) != 0;
                        })) {
            continue;
        }
        if (std::any_of(layers.begin(), layers.end(),
                        [reqLayer](const char *layer) { return strcmp(reqLayer, layer) == 0; })) {
            continue;
        }
        layers.push_back(reqLayer);
    }
    // extensions
    std::vector<const char *> exts;
    std::vector<const char *> requiredExts = {"VK_EXT_swapchain_colorspace"};
    if (pfn2 != nullptr) {
        requiredExts.push_back("VK_EXT_debug_utils");
    }
    for (uint32_t i = 0; i < wndExtensionCount; ++i) {
        exts.push_back(wndExtensionNames[i]);
    }
    for (const auto &reqExt : requiredExts) {
        if (std::all_of(extensionProperties.begin(), extensionProperties.end(),
                        [reqExt](const vk::ExtensionProperties &property) {
                            return strcmp(reqExt, property.extensionName.data()) != 0;
                        })) {
            continue;
        }
        if (std::any_of(exts.begin(), exts.end(),
                        [reqExt](const char *ext) { return strcmp(reqExt, ext) == 0; })) {
            continue;
        }
        exts.push_back(reqExt);
    }

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG
    outstr = "enabled layers:";
    for (const auto &layer : layers) {
        outstr = VKS_FORMAT::format("{}\n\t{}", outstr, layer);
    }
    VKS_RENDER_LOG_DEBUG("{}", outstr);
    outstr = "enabled extensions:";
    for (const auto &ext : exts) {
        outstr = VKS_FORMAT::format("{}\n\t{}", outstr, ext);
    }
    VKS_RENDER_LOG_DEBUG("{}", outstr);
#endif

    vk::DebugUtilsMessengerCreateInfoEXT debugMessengerInfo(
        {},
        vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
            vk::DebugUtilsMessageSeverityFlagBitsEXT::eError,
        vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
            vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance |
            vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation,
        pfn2);
    vk::ApplicationInfo    appinfo(appName.data(), appVersion, "vulkan-sample", // NOLINT
                                   vk::makeApiVersion(0, 1, 0, 0), vk::ApiVersion13);
    vk::InstanceCreateInfo info({}, &appinfo, layers, exts);

    if (pfn2 != nullptr) {
        info.setPNext(&debugMessengerInfo);
    }
    if (res = vk::createInstance(&info, nullptr, &instance); res != vk::Result::eSuccess) {
        errc = make_error_code(res);
        return std::pair(instance, messenger);
    }
    VULKAN_HPP_DEFAULT_DISPATCHER.init(instance);
    if (pfn2 == nullptr) {
        return std::pair(instance, messenger);
    }
    if (res = instance.createDebugUtilsMessengerEXT(&debugMessengerInfo, nullptr, &messenger);
        res != vk::Result::eSuccess) {
        errc = make_error_code(res);
        return std::pair(instance, messenger);
    }
    return std::pair(instance, messenger);
}

auto create_instance_and_debug_messenger_unique(PFN_vkGetInstanceProcAddr            pfn1,       //
                                                PFN_vkDebugUtilsMessengerCallbackEXT pfn2,       //
                                                std::string_view                     appName,    //
                                                uint32_t                             appVersion, //
                                                uint32_t           wndExtensionCount,            //
                                                const char *const *wndExtensionNames,            //
                                                std::error_code   &errc                          //
                                                )
    -> std::pair<vk::UniqueInstance, vk::UniqueDebugUtilsMessengerEXT>
{
    auto [instance, messenger] = create_instance_and_debug_messenger(
        pfn1, pfn2, appName, appVersion, wndExtensionCount, wndExtensionNames, errc);
    return std::pair(
        vk::UniqueInstance(instance, vk::ObjectDestroy<vk::NoParent, vk::DispatchLoaderDynamic>(
                                         nullptr, VULKAN_HPP_DEFAULT_DISPATCHER)),
        vk::UniqueDebugUtilsMessengerEXT(messenger,
                                         vk::ObjectDestroy<vk::Instance, vk::DispatchLoaderDynamic>(
                                             instance, nullptr, VULKAN_HPP_DEFAULT_DISPATCHER)));
}

auto create_logical_device(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface,
                           std::error_code &errc) -> vk::Device
{
    vk::Result res;
    vk::Device device;

    auto layerProperties     = physicalDevice.enumerateDeviceLayerProperties().value;
    auto extensionProperties = physicalDevice.enumerateDeviceExtensionProperties().value;
    auto outstr              = std::string();

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE
    outstr = VKS_FORMAT::format("{} has layers:", physicalDevice.getProperties().deviceName.data());
    for (const auto &property : layerProperties) {
        outstr = VKS_FORMAT::format(
            "{}\n\tname: {}, des: {}, impl version: {}.{}.{}.{}, spec version: {}.{}.{}.{}", outstr,
            property.layerName.data(), property.description.data(),
            vk::apiVersionMajor(property.implementationVersion),
            vk::apiVersionMinor(property.implementationVersion),
            vk::apiVersionPatch(property.implementationVersion),
            vk::apiVersionVariant(property.implementationVersion),
            vk::apiVersionMajor(property.specVersion), vk::apiVersionMinor(property.specVersion),
            vk::apiVersionPatch(property.specVersion), vk::apiVersionVariant(property.specVersion));
    }
    VKS_RENDER_LOG_TRACE("{}", outstr);
    outstr =
        VKS_FORMAT::format("{} has extensions:", physicalDevice.getProperties().deviceName.data());
    for (const auto &property : extensionProperties) {
        outstr = VKS_FORMAT::format(
            "{}\n\tname: {}, spec version: {}.{}.{}.{}", outstr, property.extensionName.data(),
            vk::apiVersionMajor(property.specVersion), vk::apiVersionMinor(property.specVersion),
            vk::apiVersionPatch(property.specVersion), vk::apiVersionVariant(property.specVersion));
    }
    VKS_RENDER_LOG_TRACE("{}", outstr);
#endif
    // layers
    std::vector<const char *> layers;
    std::vector<const char *> requiredLayers;
    for (const auto &reqLayer : requiredLayers) {
        if (std::all_of(layerProperties.begin(), layerProperties.end(),
                        [reqLayer](const vk::LayerProperties &property) {
                            return strcmp(reqLayer, property.layerName.data()) != 0;
                        })) {
            continue;
        }
        if (std::any_of(layers.begin(), layers.end(),
                        [reqLayer](const char *layer) { return strcmp(reqLayer, layer) == 0; })) {
            continue;
        }
        layers.push_back(reqLayer);
    }
    // extensions
    std::vector<const char *> exts;
    std::vector<const char *> requiredExts = {"VK_KHR_swapchain"};
    for (const auto &reqExt : requiredExts) {
        if (std::all_of(extensionProperties.begin(), extensionProperties.end(),
                        [reqExt](const vk::ExtensionProperties &property) {
                            return strcmp(reqExt, property.extensionName.data()) != 0;
                        })) {
            continue;
        }
        if (std::any_of(exts.begin(), exts.end(),
                        [reqExt](const char *ext) { return strcmp(reqExt, ext) == 0; })) {
            continue;
        }
        exts.push_back(reqExt);
    }
    // feature
    vk::PhysicalDeviceFeatures feature;

    std::vector<vk::DeviceQueueCreateInfo> queueInfos;

    vk::DeviceCreateInfo info({}, queueInfos, layers, exts, &feature);
    if (res = physicalDevice.createDevice(&info, nullptr, &device); res != vk::Result::eSuccess) {
        errc = make_error_code(res);
        return device;
    }
    return device;
}

auto create_logical_device_unique(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface,
                                  std::error_code &errc) -> vk::UniqueDevice
{
    auto device = create_logical_device(physicalDevice, surface, errc);
    return vk::UniqueDevice(device, vk::ObjectDestroy<vk::NoParent, vk::DispatchLoaderDynamic>(
                                        nullptr, VULKAN_HPP_DEFAULT_DISPATCHER));
}

VKS_RENDER_END
VKS_END