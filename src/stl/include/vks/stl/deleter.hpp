//
// deleter.hpp
//
// Package: vulkan sample
// Library: VksStl
// Module:  stl
//

#pragma once

#include "vks/basic/osdep.h"
#include "vks/stl/stl_library.h"

#include <cstdio>

VKS_BEGIN
VKS_STL_BEGIN

struct VKS_STL_API FileDeleter {
    void operator()(FILE *file);
};

VKS_STL_END
VKS_END