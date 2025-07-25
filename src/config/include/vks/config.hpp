#pragma once
#include <vks/__config/os.hpp>
#include <vks/__config/compiler.hpp>
#include <vks/__config/platform.hpp>

#define VKS_VERSION                                                                                \
    ((((uint32_t)(VKS_VERSION_BUILD)) << 29U) | (((uint32_t)(VKS_VERSION_MAJOR)) << 22U) |         \
     (((uint32_t)(VKS_VERSION_MINOR)) << 12U) | ((uint32_t)(VKS_VERSION_ALTER)))
#define VKS_VERSION_STR "0.0.1.1"
#define VKS_VERSION_MAJOR 0
#define VKS_VERSION_MINOR 0
#define VKS_VERSION_ALTER 1
#define VKS_VERSION_BUILD 1
#define VKS_PLAT "WINDOWS"
#define VKS_ARCH "X64"
#define VKS_SHARED_BUILD 1
#define VKS_MODE "DEBUG"
#define VKS_DEBUG 1
#define VKS_OS "WINDOWS"

#define VKS_BEGIN                                                                                  \
    namespace vks                                                                                  \
    {
#define VKS_END }
