#pragma once
#include <vks/config.hpp>
#include <vks/__wsi/__config.hpp>
// Standard Library
#include <system_error>

VKS_BEGIN
VKS_WSI_BEGIN

struct VKS_WSI_API Context {
    Context() = default;
    ~Context();

    Context(const Context &)            = delete;
    Context &operator=(const Context &) = delete;

    Context(Context &&rhs) noexcept;
    Context &operator=(Context &&rhs) noexcept;

    [[nodiscard("You should handle errors that may occur.")]]
    auto init(uint32_t initFlags) -> std::error_code;
    void quit() const;

protected:
    uint32_t _initFlags;
};

VKS_WSI_END
VKS_END