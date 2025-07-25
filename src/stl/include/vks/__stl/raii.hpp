#pragma once
#include <vks/config.hpp>
#include <vks/__stl/__config.hpp>
// Standard Library
#include <cstdio>
#include <memory>

VKS_BEGIN
VKS_STL_BEGIN

struct VKS_STL_API FileCloser {
    void operator()(FILE *file);
};

namespace raii
{
    using File = std::unique_ptr<FILE, FileCloser>;
}

VKS_STL_END
VKS_END