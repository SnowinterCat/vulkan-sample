#pragma once
#include <vks/config.hpp>
// Standard Library
#include <version>

// VKS_STL_API
#if defined(_WIN32) && defined(VKS_SHARED_BUILD)
    #if defined(VKS_STL_COMPILING)
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

// fmtlib
#if __cpp_lib_format < 202311L
    #include <fmt/xchar.h>
#endif
namespace fmtlib
{
#if __cpp_lib_format >= 202311L
    using namespace std;
#else
    using namespace fmt;
#endif
} // namespace fmtlib

// outptrlib
#ifndef __cpp_lib_out_ptr
    #include <ztd/out_ptr.hpp>
#endif
namespace outptrlib
{
#if defined(__cpp_lib_out_ptr)
    using namespace std;
#else
    using namespace ztd::out_ptr;
#endif
} // namespace outptrlib

// expectedlib
#ifndef __cpp_lib_expected
    #include <tl/expected.hpp>
#endif
namespace expectedlib
{
#if defined(__cpp_lib_expected)
    using namespace std;
#else
    using namespace tl;
#endif
} // namespace expectedlib
