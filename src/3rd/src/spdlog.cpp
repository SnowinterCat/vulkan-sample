#include "vks/3rd/spdlog.hpp"

#include <spdlog/spdlog.h>
#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>

constexpr uint64_t LogFileSize  = 1024ULL * 1024 * 10; // 单个日志文件大小
constexpr uint64_t LogFileCount = 10ULL;               // 滚动日志数量上限

VKS_BEGIN
VKS_THIRD_PARTY_BEGIN

std::string log_init(const std::filesystem::path &logPath, const std::string &loggerName,
                     const std::string &pattern)
{
    std::string logName = loggerName.empty() ? logPath.stem().string() : loggerName;

    std::vector<spdlog::sink_ptr> sinks = {};
    sinks.push_back(std::make_shared<spdlog::sinks::stdout_color_sink_mt>());
    sinks.push_back(std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
        logPath.string(), LogFileSize, LogFileCount));
    auto logger = std::make_shared<spdlog::logger>(logName, sinks.begin(), sinks.end());
    spdlog::set_default_logger(logger);
    spdlog::set_pattern(pattern);
    spdlog::set_level(spdlog::level::trace);
    spdlog::flush_on(spdlog::level::info);
    spdlog::set_error_handler(
        [](const std::string &msg) { SPDLOG_ERROR("*** LOGGER ERROR ***: {}", msg); }); // NOLINT
    // spdlog::flush_every(std::chrono::seconds(5));
    return logName;
}

void log_uninit(const std::string &loggerName)
{
    spdlog::drop(loggerName);
}

VKS_THIRD_PARTY_END
VKS_END