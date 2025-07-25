#pragma once
#if defined(_WIN32) && defined(VKS_DLL)
    #if defined(VKS_STL_EXPORTS)
        #define VKS_STL_API __declspec(dllexport)
    #else
        #define VKS_STL_API __declspec(dllimport)
    #endif
#endif

#if !defined(VKS_STL_API)
    #if !defined(VKS_NO_GCC_API_ATTRIBUTE) && defined(__GNUC__) && (__GNUC__ >= 4)
        #define VKS_STL_API __attribute__((visibility("default")))
    #else
        #define VKS_STL_API
    #endif
#endif

#define VKS_STL_BEGIN                                                                              \
    namespace stl                                                                                  \
    {
#define VKS_STL_END }

// clang-format off
#define VKS_STL_LOG_TRACE(...)    SPDLOG_TRACE(__VA_ARGS__)
#define VKS_STL_LOG_DEBUG(...)    SPDLOG_DEBUG(__VA_ARGS__)
#define VKS_STL_LOG_INFO(...)     SPDLOG_INFO(__VA_ARGS__)
#define VKS_STL_LOG_WARN(...)     SPDLOG_WARN(__VA_ARGS__)
#define VKS_STL_LOG_ERROR(...)    SPDLOG_ERROR(__VA_ARGS__)
#define VKS_STL_LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
// clang-format on
