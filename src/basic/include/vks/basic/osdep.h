//
// osdep.h
//
// Package: vulkan sample
// Library: VksBasic
// Module:  Basic
//

#ifndef _VULKAN_SAMPLE_BASIC_OSDEP_H_
#define _VULKAN_SAMPLE_BASIC_OSDEP_H_

#include "vks/basic/compiler.h"

#ifdef _WIN32
#    ifndef _WIN32_WINNT
#        define _WIN32_WINNT 0x0601 // support Win7
#    endif
#    ifndef WIN32_LEAN_AND_MEAN
#        define WIN32_LEAN_AND_MEAN
#    endif
#    ifndef NOMINMAX
#        define NOMINMAX
#    endif
#    ifndef UNICODE
#        define UNICODE
#    endif
#    ifndef _UNICODE
#        define _UNICODE
#    endif
#    ifndef _CRT_SECURE_NO_WARNINGS
#        define _CRT_SECURE_NO_WARNINGS
#    endif
#    ifndef _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#        define _SILENCE_CXX17_CODECVT_HEADER_DEPRECATION_WARNING
#    endif
#endif

#define VKS_NAMESAPCE vks
#define VKS_NAMESAPCE_STRING "vks"
#define VKS_BEGIN                                                                                  \
    namespace vks                                                                                  \
    {
#define VKS_END }

#endif