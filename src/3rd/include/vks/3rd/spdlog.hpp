//
// spdlog.hpp
//
// Package: vulkan sample
// Library: VksThirdParty
// Module:  thirdParty
//

#pragma once

#include "vks/basic/osdep.h"
#include "vks/3rd/3rd_library.h"

#include <string>
#include <filesystem>

VKS_BEGIN
VKS_THIRD_PARTY_BEGIN

/**
 * @brief 初始化日志记录器，并返回日志记录器的标识名
 *
 * @param logPath 日志文件的路径
 * @param loggerName 日志记录器标识名，为空时默认使用日志文件的无后缀字符串作为标识名
 * @param pattern 日志格式，为空时默认使用 "[%Y-%m-%d %T.%f] [%n] [%P] [%t] %^[%l]%$ [%s:%#]\n%v"
 * @return std::string 日志标识名，当 loggerName
 * 有值时会直接返回原值，否则返回日志文件的无后缀字符串作为标识名
 */
VKS_THIRD_PARTY_API std::string log_init(
    const std::filesystem::path &logPath,         //
    const std::string           &loggerName = {}, //
    const std::string           &pattern = "[%Y-%m-%d %T.%f] [%n] [%P] [%t] %^[%l]%$ [%s:%#]\n%v");

/**
 * @brief 使用日志记录器的标识名，删除日志记录器
 *
 * @param loggerName 日志记录器标识名
 * @return void
 */
VKS_THIRD_PARTY_API void log_uninit(const std::string &loggerName);

VKS_THIRD_PARTY_END
VKS_END