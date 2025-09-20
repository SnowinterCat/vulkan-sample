#include <vks/wsi/init.hpp>
// Standard Library
// Third-party Library
#include <SDL3/SDL.h>
//
#include <vks/stl/scope.hpp>

VKS_BEGIN
VKS_WSI_BEGIN

Context::~Context()
{
    quit();
}

Context::Context(Context &&rhs) noexcept : _initFlags(std::exchange(rhs._initFlags, {})) {}
Context &Context::operator=(Context &&rhs) noexcept
{
    if (this != &rhs) {
        std::swap(_initFlags, rhs._initFlags);
    }
    return *this;
}

auto Context::init(uint32_t initFlags) -> std::error_code
{
    if (initFlags == 0U) {
        return std::error_code();
    }
    _initFlags = SDL_InitSubSystem(initFlags) ? initFlags : static_cast<uint32_t>(0);
    return (_initFlags != 0U) ? std::error_code() : std::make_error_code(std::errc::interrupted);
}
void Context::quit() const
{
    if (_initFlags != 0U) {
        SDL_QuitSubSystem(_initFlags);
    }
}

VKS_WSI_END
VKS_END