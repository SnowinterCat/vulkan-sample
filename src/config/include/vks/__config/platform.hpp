#pragma once
#include <vks/config.hpp>

// define VKS_FORCE_EXPORT_API
#if defined(_WIN32)
    #define VKS_FORCE_EXPORT_API __declspec(dllexport)
#endif
#if !defined(VKS_FORCE_EXPORT_API)
    #if defined(__GNUC__) && (__GNUC__ >= 4)
        #define VKS_FORCE_EXPORT_API __attribute__((visibility("default")))
    #else
        #define VKS_FORCE_EXPORT_API
    #endif
#endif
