//
// win/error_code.h
//
// Package: vulkan sample
// Library: VksPlatform
// Module:  platform
//

#pragma once

#include "vks/basic/osdep.h"
#include "vks/platform/platform_library.h"
// standard library
#include <string>
#include <system_error>
// system library
#include <Windows.h>

VKS_BEGIN
VKS_PLATFORM_BEGIN

VKS_PLATFORM_API std::string win32_error_to_string(int dwErrorCode);

class ErrorCategoryWin : public std::error_category { // categorize a generic error
public:
    constexpr ErrorCategoryWin() noexcept : error_category() {}

    const char *name() const noexcept override
    {
        return "::" VKS_NAMESAPCE_STRING "::platform::ErrorCategoryWin";
    }
    std::string message(int errcode) const override { return win32_error_to_string(errcode); }
};

VKS_PLATFORM_API const std::error_category &win32error_category() noexcept;

inline std::error_code make_error_code(HRESULT ec) noexcept
{
    return std::error_code(static_cast<int>(ec), win32error_category());
}

inline std::error_code make_error_code(DWORD ec) noexcept
{
    return std::error_code(static_cast<int>(ec), win32error_category());
}

VKS_PLATFORM_END
VKS_END
