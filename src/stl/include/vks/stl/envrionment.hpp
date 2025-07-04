//
// envrionment.hpp
//
// Package: vulkan sample
// Library: VksStl
// Module:  stl
//

#pragma once

#include "vks/basic/osdep.h"
#include "vks/stl/stl_library.h"

#include <string>
#include <vector>

VKS_BEGIN
VKS_STL_BEGIN

/**
 * @brief 将 unicode(wchar_t)编码 的 argv 转换成 utf8(char)编码 的 argv
 *
 * @param argc 参数个数
 * @param wargv unicode(wchar_t)编码 的参数字符串指针列表的指针
 * @param args 转换后的 utf8(char)编码 的参数内容
 * @param argvector 转换后的 utf8(char)编码 的参数字符串指针列表
 * @return char** 转换后的 utf8(char)编码 的参数字符串指针列表的指针
 * @remarks 若 args
 * 非空，则转换后的内容会接在原内容之后，但原内容不会被当作参数。不建议出于此目的使用该函数，此功能仅为了保证当
 * args 非空时也能正常工作
 */
VKS_STL_API char **convert_argc_argv(size_t argc, const wchar_t *const *wargv, std::string &args,
                                     std::vector<char *> &argvector);

/**
 * @brief
 *
 * @param str
 * @param argc
 * @param argvector
 * @return std::string
 */
VKS_STL_API std::string string_to_argc_argv(const std::string &str, int &argc,
                                            std::vector<const char *> &argvector);

VKS_STL_END
VKS_END