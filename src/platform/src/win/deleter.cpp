#include "vks/platform/win/deleter.hpp"

#pragma comment(lib, "user32.lib")
#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "advapi32.lib")
#pragma comment(lib, "userenv.lib")

VKS_BEGIN
VKS_PLATFORM_BEGIN

////////////////////////////////////////////////////////////////////////////////
// deleter
////////////////////////////////////////////////////////////////////////////////

void WinAddressInfoWDeleter::operator()(PADDRINFOW resw) const
{
    if (resw != nullptr) {
        FreeAddrInfoW(resw);
    }
}

void WinDeskDeleter::operator()(HDESK hdesk) const
{
    if (hdesk != nullptr) {
        CloseDesktop(hdesk);
    }
}

void WinEnvironmentBlockDeleter::operator()(void *lpEnvironment) const
{
    if (lpEnvironment != nullptr) {
        DestroyEnvironmentBlock(lpEnvironment);
    }
}

void WinHandleDeleter::operator()(HANDLE handle) const
{
    if (handle != nullptr && handle != INVALID_HANDLE_VALUE) {
        CloseHandle(handle);
    }
}

void WinKeyDeleter::operator()(HKEY hKey) const
{
    if (hKey != nullptr) {
        RegCloseKey(hKey);
    }
}

void WinModuleDeleter::operator()(HMODULE hmodule) const
{
    if (hmodule != nullptr) {
        FreeLibrary(hmodule);
    }
}

void WinLocalFreeDeleter::operator()(void *pointer) const
{
    if (pointer != nullptr) {
        LocalFree(pointer);
    }
}

void WinMutexResetor::operator()(HANDLE mutex) const
{
    if (mutex != nullptr && mutex != INVALID_HANDLE_VALUE) {
        ReleaseMutex(mutex);
    }
}

void WinPipeDisconnector::operator()(HANDLE handle) const
{
    if (handle != nullptr && handle != INVALID_HANDLE_VALUE) {
        CancelIo(handle), DisconnectNamedPipe(handle);
    }
}

void WinServiceHandleDeleter::operator()(SC_HANDLE scObj) const
{
    if (scObj != nullptr && scObj != INVALID_HANDLE_VALUE) {
        CloseServiceHandle(scObj);
    }
}

void WinSidDeleter::operator()(void *psid) const
{
    if (psid != nullptr) {
        FreeSid(psid);
    }
}

void WinWndDestroyer::operator()(HWND hwnd) const
{
    if (hwnd != nullptr) {
        DestroyWindow(hwnd);
    }
}

void *WinWSAStartupper::operator()(unsigned short wVersionRequested) const
{
    WSADATA wsaData = {};
    return WSAStartup(wVersionRequested, &wsaData) != 0 ? nullptr : reinterpret_cast<void *>(1);
}

void WinWSACleanupper::operator()([[maybe_unused]] void *pointer) const
{
    if (pointer != nullptr) {
        WSACleanup();
    }
}

VKS_PLATFORM_END
VKS_END
