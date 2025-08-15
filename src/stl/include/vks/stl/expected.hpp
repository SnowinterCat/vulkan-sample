#pragma once
#include <vks/config.hpp>
#include <vks/__stl/__config.hpp>
// Standard Library
#include <version>
// expectedlib
#if defined(__cpp_lib_expected)
    #include <expected>
#else
    #include <tl/expected.hpp>
#endif

VKS_BEGIN
VKS_STL_BEGIN

namespace expected
{
#if defined(__cpp_lib_expected)
    using namespace ::std;
#else
    using namespace ::tl;
#endif
} // namespace expected

VKS_STL_END
VKS_END