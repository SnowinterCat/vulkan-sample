//
// compiler.h
//
// Package: vulkan sample
// Library: VksBasic
// Module:  Basic
//

#ifndef _VULKAN_SAMPLE_BASIC_COMPILER_H_
#define _VULKAN_SAMPLE_BASIC_COMPILER_H_

#define VKS_HOST_BIG_ENDIAN (__BYTE_ORDER__ == __ORDER_BIG_ENDIAN__)

/* HOST_LONG_BITS is the size of a native pointer in bits. */
#define VKS_HOST_LONG_BITS (__SIZEOF_POINTER__ * 8)

#ifdef __cplusplus
#    define VKS_EXTERN_C extern "C"
#    define VKS_EXTERN_C_START                                                                     \
        extern "C"                                                                                 \
        {
#    define VKS_EXTERN_C_END }
#else
#    define VKS_EXTERN_C extern
#    define VKS_EXTERN_C_START
#    define VKS_EXTERN_C_END
#endif

#if defined(_WIN32) && !defined(__clang__) && !defined(__MINGW32__)
#    define VKS_PACKED
#    define VKS_ALIGN8 __declspec(align(8))
#    define VKS_ALIGN16 __declspec(align(16))
#    define VKS_ALIGN(X) __declspec(align(X))
#else
#    define VKS_PACKED __attribute__((packed))
#    define VKS_ALIGN8 __attribute__((aligned(8)))
#    define VKS_ALIGN16 __attribute__((aligned(16)))
#    define VKS_ALIGN(X) __attribute__((aligned(X)))
#endif

// How to packed with msvc:

// #if defined(_WIN32) && !defined(__clang__) && !defined(__MINGW32__)
// #pragma pack(push, 1)
// #endif
// typedef struct A {
//     uint8_t value1;
// } VKS_PACKED A;
// struct VKS_PACKED A {
//     uint8_t value1;
// };
// #if defined(_WIN32) && !defined(__clang__) && !defined(__MINGW32__)
// #pragma pack(pop)
// #endif

#endif