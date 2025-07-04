//
// 3rd_library.h
//
// Package: vulkan sample
// Library: VksThirdParty
// Module:  thirdParty
//

#ifndef _VULKAN_SAMPLE_THIRD_PARTY_LIBRARY_H_
#define _VULKAN_SAMPLE_THIRD_PARTY_LIBRARY_H_

// clang-format off
#if defined(_WIN32) && defined(VKS_DLL)
	#if defined(VKS_THIRD_PARTY_EXPORTS)
		#define VKS_THIRD_PARTY_API __declspec(dllexport)
	#else
		#define VKS_THIRD_PARTY_API __declspec(dllimport)
	#endif
#endif

#if !defined(VKS_THIRD_PARTY_API)
	#if !defined(VKS_NO_GCC_API_ATTRIBUTE) && defined (__GNUC__) && (__GNUC__ >= 4)
		#define VKS_THIRD_PARTY_API __attribute__((visibility ("default")))
	#else
		#define VKS_THIRD_PARTY_API
	#endif
#endif
// clang-format on

#define VKS_THIRD_PARTY_BEGIN                                                                      \
    namespace tp                                                                                   \
    {
#define VKS_THIRD_PARTY_END }

// clang-format off
#define VKS_THIRD_PARTY_LOG_TRACE(...)    SPDLOG_TRACE(__VA_ARGS__)
#define VKS_THIRD_PARTY_LOG_DEBUG(...)    SPDLOG_DEBUG(__VA_ARGS__)
#define VKS_THIRD_PARTY_LOG_INFO(...)     SPDLOG_INFO(__VA_ARGS__)
#define VKS_THIRD_PARTY_LOG_WARN(...)     SPDLOG_WARN(__VA_ARGS__)
#define VKS_THIRD_PARTY_LOG_ERROR(...)    SPDLOG_ERROR(__VA_ARGS__)
#define VKS_THIRD_PARTY_LOG_CRITICAL(...) SPDLOG_CRITICAL(__VA_ARGS__)
// clang-format on

#endif
