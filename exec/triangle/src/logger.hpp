#pragma once

// clang-format off
#define TRI_LOG_TRACE(...)      SPDLOG_TRACE(__VA_ARGS__)
#define TRI_LOG_DEBUG(...)      SPDLOG_DEBUG(__VA_ARGS__)
#define TRI_LOG_INFO(...)       SPDLOG_INFO(__VA_ARGS__)
#define TRI_LOG_WARN(...)       SPDLOG_WARN(__VA_ARGS__)
#define TRI_LOG_ERROR(...)      SPDLOG_ERROR(__VA_ARGS__)
#define TRI_LOG_CRITICAL(...)   SPDLOG_CRITICAL(__VA_ARGS__...)
// clang-format on