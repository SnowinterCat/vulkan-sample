//
// render.h
//
// Package: vulkan sample
// Library: VksRender
// Module:  Render
//

#pragma once

#include "vks/basic/osdep.h"
#include "vks/render/render_library.h"

#include <system_error>

#define VULKAN_HPP_NO_EXCEPTIONS true
#define VULKAN_HPP_DISPATCH_LOADER_DYNAMIC true
#define VULKAN_HPP_STORAGE_SHARED true
#include <vulkan/vulkan.hpp>
#include <vk_mem_alloc.hpp>

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

VKS_BEGIN
VKS_RENDER_BEGIN

class ErrorCategoryVk : public std::error_category {
public:
    virtual const char *name() const noexcept override
    {
        return "::" VKS_NAMESAPCE_STRING "::vk::Result";
    }
    virtual std::string message(int errc) const override
    {
        return ::vk::to_string(static_cast<::vk::Result>(errc));
    }
};

inline const std::error_category &error_category_vk() noexcept
{
    static constexpr ErrorCategoryVk Instance;
    return Instance;
}

inline std::error_code make_error_code(vk::Result errc) noexcept
{
    return std::error_code(static_cast<int>(errc), error_category_vk());
}

inline std::error_condition make_error_condition(vk::Result errc) noexcept
{
    return std::error_condition(static_cast<int>(errc), error_category_vk());
}

VKS_RENDER_END
VKS_END

#include "vks/render/handles.hpp"
#include "vks/render/structs.hpp"
#include "vks/render/funcs.hpp"