#include "vks/3rd/libsdl3.hpp"

VKS_BEGIN
VKS_THIRD_PARTY_BEGIN

void SDLQuiter::operator()(void *res) const
{
    if (res != nullptr) {
        SDL_Quit();
    }
}

void SDLWindowDeleter::operator()(SDL_Window *window) const
{
    if (window != nullptr) {
        SDL_DestroyWindow(window);
    }
}

VKS_THIRD_PARTY_END
VKS_END