//
// win/deleter.h
//
// Package: vulkan sample
// Library: VksPlatform
// Module:  platform
//

#pragma once

#include "vks/basic/osdep.h"
#include "vks/platform/platform_library.h"
// standard library
#include <memory>
// system library
#include <WS2tcpip.h>
#include <Windows.h>
#include <iphlpapi.h>
#include <UserEnv.h>

VKS_BEGIN
VKS_PLATFORM_BEGIN

////////////////////////////////////////////////////////////////////////////////
// deleter
////////////////////////////////////////////////////////////////////////////////

struct VKS_PLATFORM_API WinAddressInfoWDeleter {
    void operator()(PADDRINFOW resw) const;
};
using Win_UniqueAddressInfoW = std::unique_ptr<addrinfoW, WinAddressInfoWDeleter>;

struct VKS_PLATFORM_API WinDeskDeleter {
    void operator()(HDESK hdesk) const;
};
using Win_UniqueDesk = std::unique_ptr<std::remove_pointer_t<HDESK>, WinDeskDeleter>;

struct VKS_PLATFORM_API WinEnvironmentBlockDeleter {
    void operator()(void *lpEnvironment) const;
};
using Win_UniqueEnvironmentBlock = std::unique_ptr<void, WinEnvironmentBlockDeleter>;

struct VKS_PLATFORM_API WinHandleDeleter {
    void operator()(HANDLE handle) const;
};
using Win_UniqueHandle = std::unique_ptr<std::remove_pointer_t<HANDLE>, WinHandleDeleter>;

struct VKS_PLATFORM_API WinKeyDeleter {
    void operator()(HKEY hKey) const;
};
using Win_UniqueKey = std::unique_ptr<std::remove_pointer_t<HKEY>, WinKeyDeleter>;

struct VKS_PLATFORM_API WinModuleDeleter {
    void operator()(HMODULE hmodule) const;
};
using Win_UniqueModule = std::unique_ptr<std::remove_pointer_t<HMODULE>, WinModuleDeleter>;

struct VKS_PLATFORM_API WinLocalFreeDeleter {
    void operator()(void *pointer) const;
};
using Win_UniqueLocal = std::unique_ptr<void, WinLocalFreeDeleter>;

struct VKS_PLATFORM_API WinMutexResetor {
    void operator()(HANDLE mutex) const;
};
using Win_UniqueMutex = std::unique_ptr<std::remove_pointer_t<HANDLE>, WinMutexResetor>;

struct VKS_PLATFORM_API WinPipeDisconnector {
    void operator()(HANDLE handle) const;
};
using Win_UniquePipeConnector = std::unique_ptr<std::remove_pointer_t<HANDLE>, WinPipeDisconnector>;

struct VKS_PLATFORM_API WinServiceHandleDeleter {
    void operator()(SC_HANDLE scObj) const;
};
using Win_UniqueServiceHandle =
    std::unique_ptr<std::remove_pointer_t<SC_HANDLE>, WinServiceHandleDeleter>;

using WinSidStringDeleter = WinLocalFreeDeleter;
using Win_UniqueSidString = std::unique_ptr<void, WinSidStringDeleter>;

struct VKS_PLATFORM_API WinSidDeleter {
    void operator()(void *psid) const;
};
using Win_UniqueSid = std::unique_ptr<void, WinSidDeleter>;

struct VKS_PLATFORM_API WinWndDestroyer {
    void operator()(HWND hwnd) const;
};
using Win_UniqueWindow = std::unique_ptr<std::remove_pointer_t<HWND>, WinWndDestroyer>;

struct VKS_PLATFORM_API WinWSAStartupper {
    void *operator()(unsigned short wVersionRequested) const;
};

struct VKS_PLATFORM_API WinWSACleanupper {
    void operator()(void *pointer) const;
};
using Win_UniqueWsa = std::unique_ptr<void, WinWSACleanupper>;

VKS_PLATFORM_END
VKS_END