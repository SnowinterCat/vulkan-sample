#pragma once

#include "vks/render/render.hpp"

VKS_BEGIN
VKS_RENDER_BEGIN

////////////////////////////////////////////////////////////////////////////////
/// default function
////////////////////////////////////////////////////////////////////////////////

VKS_RENDER_API VKAPI_ATTR VkBool32 VKAPI_CALL default_debug_messenger_callback(
    VkDebugUtilsMessageSeverityFlagBitsEXT      messageSeverity,
    VkDebugUtilsMessageTypeFlagsEXT             messageTypes,
    VkDebugUtilsMessengerCallbackDataEXT const *pCallbackData, void *pUserData);

/**
 * @brief 提供可用于 VkCreateDevice 设备队列信息（运行速度快，但分配的设备队列信息较为低效）
 *
 * @param physicalDevice
 * @param surface
 * @param queuePriorities 输出时存储着各个队列的优先级信息，可以在 VkCreateDevice 之后删除
 * @param graphicsQueueInfos 输入时表示需求的 graphics 队列的个数，及各自优先级[0,255]；输出时见备注
 * @param presentQueueInfos 输入时表示需求的 present 队列的个数，及各自优先级[0,255]；输出时见备注
 * @param computeQueueInfos 输入时表示需求的 compute 队列的个数，及各自优先级[0,255]；输出时见备注
 * @param transferQueueInfos 输入时表示需求的 transfer 队列的个数，及各自优先级[0,255]；输出时见备注
 * @return std::vector<vk::DeviceQueueCreateInfo> 可用于 VkCreateDevice 的设备队列信息
 * @brief 【此函数会认为靠后的队列不重要，因此靠前的队列尽可能不会被共享，靠后的在队列可能会共享】
 * 此函数使用贪心算法，尽可能快地分配设备队列
 * 对于参数 graphicsQueueInfo, presentQueueInfo, computeQueueInfo, transferQueueInfos 在输出时：
 * vk::apiVersionMajor(xxxQueueInfo[i]) 表示分配到的第 i 个队列的 queueFamilyIndex
 * vk::apiVersionMinor(xxxQueueInfo[i]) 表示分配到的第 i 个队列的 queueIndex
 * vk::apiVersionPatch(xxxQueueInfo[i]) 表示分配到的第 i 个队列的 优先级（等于输入值）
 */
VKS_RENDER_API auto default_queue_create_info_inefficient(
    vk::PhysicalDevice     physicalDevice,     //
    vk::SurfaceKHR         surface,            //
    std::vector<float>    &queuePriorities,    //
    std::vector<uint32_t> &graphicsQueueInfos, //
    std::vector<uint32_t> &presentQueueInfos,  //
    std::vector<uint32_t> &computeQueueInfos,  //
    std::vector<uint32_t> &transferQueueInfos, //
    std::error_code       &errc                //
    ) -> std::vector<vk::DeviceQueueCreateInfo>;

/**
 * @brief 提供可用于 VkCreateDevice 设备队列信息（运行速度慢，但分配的设备队列信息较为高效）
 *
 * @param physicalDevice
 * @param surface
 * @param queuePriorities 输出时存储着各个队列的优先级信息，可以在 VkCreateDevice 之后删除
 * @param graphicsQueueInfos 输入时表示需求的 graphics 队列的个数，及各自优先级[0,255]；输出时见备注
 * @param presentQueueInfos 输入时表示需求的 present 队列的个数，及各自优先级[0,255]；输出时见备注
 * @param computeQueueInfos 输入时表示需求的 compute 队列的个数，及各自优先级[0,255]；输出时见备注
 * @param transferQueueInfos 输入时表示需求的 transfer 队列的个数，及各自优先级[0,255]；输出时见备注
 * @return std::vector<vk::DeviceQueueCreateInfo> 可用于 VkCreateDevice 的设备队列信息
 * @brief 【此函数会认为靠后的队列不重要，因此靠前的队列尽可能不会被共享，靠后的在队列可能会共享】
 * 此函数使用二分图匹配算法，尽可能最优地分配设备队列
 * 对于参数 graphicsQueueInfo, presentQueueInfo, computeQueueInfo, transferQueueInfos 在输出时：
 * vk::apiVersionMajor(xxxQueueInfo[i]) 表示分配到的第 i 个队列的 queueFamilyIndex
 * vk::apiVersionMinor(xxxQueueInfo[i]) 表示分配到的第 i 个队列的 queueIndex
 * vk::apiVersionPatch(xxxQueueInfo[i]) 表示分配到的第 i 个队列的 优先级（等于输入值）
 */
VKS_RENDER_API auto default_queue_create_info_efficient(
    vk::PhysicalDevice     physicalDevice,     //
    vk::SurfaceKHR         surface,            //
    std::vector<float>    &queuePriorities,    //
    std::vector<uint32_t> &graphicsQueueInfos, //
    std::vector<uint32_t> &presentQueueInfos,  //
    std::vector<uint32_t> &computeQueueInfos,  //
    std::vector<uint32_t> &transferQueueInfos, //
    std::error_code       &errc                //
    ) -> std::vector<vk::DeviceQueueCreateInfo>;

////////////////////////////////////////////////////////////////////////////////
/// function
////////////////////////////////////////////////////////////////////////////////

VKS_RENDER_API auto make_unique_surface(vk::Instance instance, vk::SurfaceKHR surface)
    -> vk::UniqueSurfaceKHR;

/**
 * @brief Easily create both vk::Instance and vk::DebugUtilsMessengerEXT object
 *
 * @param pfn1
 * @param pfn2
 * @param appName
 * @param appVersion
 * @param wndExtensionCount
 * @param wndExtensionNames
 * @param errc
 * @return std::pair<vk::Instance, vk::DebugUtilsMessengerEXT>
 */
VKS_RENDER_API auto create_instance_and_debug_messenger(
    PFN_vkGetInstanceProcAddr            pfn1,              //
    PFN_vkDebugUtilsMessengerCallbackEXT pfn2,              //
    std::string_view                     appName,           //
    uint32_t                             appVersion,        //
    uint32_t                             wndExtensionCount, //
    const char *const                   *wndExtensionNames, //
    std::error_code                     &errc               //
    ) -> std::pair<vk::Instance, vk::DebugUtilsMessengerEXT>;

/**
 * @brief Easily create both vk::UniqueInstance and vk::UniqueDebugUtilsMessengerEXT object
 *
 * @param pfn1
 * @param pfn2
 * @param appName
 * @param appVersion
 * @param wndExtensionCount
 * @param wndExtensionNames
 * @param errc
 * @return std::pair<vk::UniqueInstance, vk::UniqueDebugUtilsMessengerEXT>
 */
VKS_RENDER_API auto create_instance_and_debug_messenger_unique(
    PFN_vkGetInstanceProcAddr            pfn1,              //
    PFN_vkDebugUtilsMessengerCallbackEXT pfn2,              //
    std::string_view                     appName,           //
    uint32_t                             appVersion,        //
    uint32_t                             wndExtensionCount, //
    const char *const                   *wndExtensionNames, //
    std::error_code                     &errc               //
    ) -> std::pair<vk::UniqueInstance, vk::UniqueDebugUtilsMessengerEXT>;

VKS_RENDER_API auto create_logical_device(vk::PhysicalDevice physicalDevice, vk::SurfaceKHR surface,
                                          std::error_code &errc) -> vk::Device;

VKS_RENDER_API auto create_logical_device_unique(vk::PhysicalDevice physicalDevice,
                                                 vk::SurfaceKHR surface, std::error_code &errc)
    -> vk::UniqueDevice;

VKS_RENDER_END
VKS_END