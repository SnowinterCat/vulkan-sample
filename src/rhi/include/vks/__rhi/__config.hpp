#pragma once
#include <vks/config.hpp>
// Standard Library
#include <version>

// VKS_RHI_API
#if defined(_WIN32) && defined(VKS_SHARED_BUILD)
    #if defined(VKS_RHI_COMPILING)
        #define VKS_RHI_API __declspec(dllexport)
    #else
        #define VKS_RHI_API __declspec(dllimport)
    #endif
#endif
#if !defined(VKS_RHI_API)
    #if !defined(VKS_NO_GCC_API_ATTRIBUTE) && defined(__GNUC__) && (__GNUC__ >= 4)
        #define VKS_RHI_API __attribute__((visibility("default")))
    #else
        #define VKS_RHI_API
    #endif
#endif

#define VKS_RHI_BEGIN                                                                              \
    namespace rhi                                                                                  \
    {
#define VKS_RHI_END }

// clang-format off
#define VKS_RHI_LOG_TRACE(...)    SPDLOG_TRACE(__VA_ARGS__)
#define VKS_RHI_LOG_DEBUG(...)    SPDLOG_DEBUG(__VA_ARGS__)
#define VKS_RHI_LOG_INFO(...)     SPDLOG_INFO(__VA_ARGS__)
#define VKS_RHI_LOG_WARN(...)     SPDLOG_WARN(__VA_ARGS__)
#define VKS_RHI_LOG_ERROR(...)    SPDLOG_ERROR(__VA_ARGS__)
#define VKS_RHI_LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
// clang-format on
