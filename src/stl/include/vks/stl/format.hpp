#pragma once
#include <vks/config.hpp>
#include <vks/__stl/__config.hpp>
// Standard Library
#include <version>
// fmtlib
#if __cpp_lib_format >= 202311L
    #include <format>
#else
    #include <fmt/xchar.h>
#endif

VKS_BEGIN
VKS_STL_BEGIN

namespace fmt
{
#if __cpp_lib_format >= 202311L
    using namespace ::std;
#else
    using namespace ::fmt;
#endif
} // namespace fmt

VKS_STL_END
VKS_END