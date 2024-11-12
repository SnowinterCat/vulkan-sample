#include "vks/basic/osdep.h"

#include <clocale>
#include <string>
#include <vector>
#include <filesystem>
#if __cplusplus > 202600L
#    include <format>
#    define VKS_FORMAT std
#else
#    include <fmt/xchar.h>
#    define VKS_FORMAT fmt
#endif

#define VULKAN_HPP_NO_EXCEPTIONS true
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC true
#define VULKAN_HPP_STORAGE_SHARED true
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>
#include <SDL3/SDL.h>
#include <SDL3/SDL_vulkan.h>
#include <spdlog/spdlog.h>

#include "vks/stl/stl.hpp"
#include "vks/3rd/3rd.hpp"
#include "vks/render/render.hpp"

#include "logger.hpp"

#ifdef _WIN32
int wmain([[maybe_unused]] int argc, [[maybe_unused]] wchar_t *wargv[])
#else
int main([[maybe_unused]] int argc, [[maybe_unused]] char *argv[])
#endif
{
    // utf-8 encoding
    std::setlocale(LC_ALL, ".utf-8");
#ifdef _WIN32
    std::string             args      = {};
    std::vector<char *>     argvector = {};
    [[maybe_unused]] char **argv      = vks::stl::convert_argc_argv(argc, wargv, args, argvector);
#endif
    // log setting
    std::filesystem::path workPath(std::filesystem::absolute(argv[0]).parent_path().parent_path());
    vks::tp::log_init(workPath / "log" / "triangle.log");

    // sdl3
    vks::tp::SDL_UniqueInit init(reinterpret_cast<void *>(SDL_Init(SDL_INIT_VIDEO)));
    if (!init) {
        TRI_LOG_ERROR("sdl init error, info: {}", SDL_GetError());
        return 0;
    }
    vks::tp::SDL_UniqueWindow window(SDL_CreateWindow("triangle", 640, 360, SDL_WINDOW_VULKAN));
    if (!window) {
        TRI_LOG_ERROR("sdl CreateWindow error, info: {}", SDL_GetError());
        return 0;
    }

    // vulkan
    uint32_t           extCnt;
    const char *const *exts = SDL_Vulkan_GetInstanceExtensions(&extCnt);
    // instance
    std::error_code errc;
    auto [instance, messenger] = vks::render::create_instance_and_debug_messenger_unique(
        reinterpret_cast<PFN_vkGetInstanceProcAddr>(SDL_Vulkan_GetVkGetInstanceProcAddr()),
        &vks::render::default_debug_messenger_callback, "triangle", vk::makeApiVersion(0, 1, 0, 0),
        extCnt, exts, errc);
    if (errc) {
        TRI_LOG_ERROR("create vulkan instance or debug messenger error, info: {}", errc.message());
    }
    // surface
    vk::UniqueSurfaceKHR surface = [](SDL_Window  *window,
                                      vk::Instance instance) -> vk::UniqueSurfaceKHR {
        VkSurfaceKHR surface;
        SDL_Vulkan_CreateSurface(window, instance, nullptr, &surface);
        return vks::render::make_unique_surface(instance, surface);
    }(window.get(), instance.get());
    // device
    std::vector<float>    queuePriorities;
    std::vector<uint32_t> graphicsInfos, presentInfos, computeInfos, transferInfos; // NOLINT

    auto physicalDevices = instance->enumeratePhysicalDevices().value;
    auto queueInfos      = vks::render::default_queue_create_info_inefficient(
        physicalDevices[0], surface.get(), queuePriorities, graphicsInfos, presentInfos,
        computeInfos, transferInfos, errc);
    auto logicalDevice =
        vks::render::create_logical_device_unique(physicalDevices[0], surface.get(), errc);
    if (errc) {
        TRI_LOG_ERROR("create vulkan logical device error, info: {}", errc.message());
    }

    bool done = false;
    while (!done) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            if (event.type == SDL_EVENT_QUIT) {
                break;
            }
            if (event.type == SDL_EVENT_WINDOW_CLOSE_REQUESTED &&
                event.window.windowID == SDL_GetWindowID(window.get())) {
                done = true;
            }
        }
    }
    return 0;
}