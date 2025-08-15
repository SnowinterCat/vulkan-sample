#pragma once
#include <vks/config.hpp>
#include <vks/__stl/__config.hpp>
// Standard Library
#include <version>
// outptrlib
#if defined(__cpp_lib_out_ptr)
    #include <memory>
#else
    #include <memory>
    #include <ztd/out_ptr.hpp>
#endif

VKS_BEGIN
VKS_STL_BEGIN

namespace outptr
{
#if defined(__cpp_lib_out_ptr)
    using namespace ::std;
#else
    using namespace ::ztd::out_ptr;
#endif
} // namespace outptr

VKS_STL_END
VKS_END