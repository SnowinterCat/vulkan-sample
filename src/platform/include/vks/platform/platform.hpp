//
// platform.hpp
//
// Package: vulkan sample
// Library: VksPlatform
// Module:  platform
//

#pragma once

#include "vks/basic/osdep.h"
#include "vks/platform/platform_library.h"

#ifdef _WIN32
#    include "vks/platform/win/deleter.hpp"
#    include "vks/platform/win/error_code.hpp"
#elif defined(__linux__)
#    include "vks/platform/unix/deleter.hpp"
#    include "vks/platform/unix/error_code.hpp"
#elif defined(__ANDROID__)
#    include "vks/platform/andr/deleter.hpp"
#    include "vks/platform/andr/error_code.hpp"
#else
#    error "not support platform"
#endif