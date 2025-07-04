#include "vks/platform/win/error_code.hpp"

#include <codecvt>
#include <format>

#pragma comment(lib, "kernel32.lib")

#include "vks/platform/win/deleter.hpp"

VKS_BEGIN
VKS_PLATFORM_BEGIN

const std::error_category &win32error_category() noexcept
{
    static constexpr ErrorCategoryWin Category;
    return Category;
}

std::string win32_error_to_string(int dwErrorCode)
{
    LPWSTR fmtstr = nullptr;
    DWORD  len    = FormatMessageW(FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS |
                                       FORMAT_MESSAGE_ALLOCATE_BUFFER,
                                   NULL, /* (not used with FORMAT_MESSAGE_FROM_SYSTEM) */
                                   dwErrorCode, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                                   reinterpret_cast<LPWSTR>(&fmtstr), 0, NULL);
    vks::plf::Win_UniqueLocal autoRelease(fmtstr);

    if (len == 0U) {
        return std::format("Error Code: {}", dwErrorCode);
    }
    std::wstring_convert<std::codecvt_utf8<wchar_t>, wchar_t> converter = {};
    return converter.to_bytes(fmtstr);
}

VKS_PLATFORM_END
VKS_END