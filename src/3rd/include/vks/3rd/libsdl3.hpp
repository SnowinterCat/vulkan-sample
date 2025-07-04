//
// libsdl3.hpp
//
// Package: vulkan sample
// Library: VksThirdParty
// Module:  thirdParty
//

#pragma once

#include "vks/basic/osdep.h"
#include "vks/3rd/3rd_library.h"

#include <memory>

#include <SDL3/SDL.h>

VKS_BEGIN
VKS_THIRD_PARTY_BEGIN

////////////////////////////////////////////////////////////////////////////////
// deleter
////////////////////////////////////////////////////////////////////////////////

struct VKS_THIRD_PARTY_API SDLQuiter {
    void operator()(void *res) const;
};
using SDL_UniqueInit = std::unique_ptr<void, SDLQuiter>;

struct VKS_THIRD_PARTY_API SDLWindowDeleter {
    void operator()(SDL_Window *window) const;
};
using SDL_UniqueWindow = std::unique_ptr<SDL_Window, SDLWindowDeleter>;

VKS_THIRD_PARTY_END
VKS_END