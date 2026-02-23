#pragma once

#include "Raysim/Utils/Memory.hpp"

// Suppress warnings from external spdlog headers
#pragma warning(push, 0)
#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>
#pragma warning(pop)

namespace RS {

// ============================================================================
// LOGGING SYSTEM
// ============================================================================

// Logger class manages the core logging functionality using spdlog
// Provides static methods for initialization and logging
class Logger
{
public:
    // Initialize logging system (call once at startup)
    // Sets up core logger with specified log file and log level
    static void Init(
        const std::string& app_name,
        const std::string& log_file,
        bool async = true);

    // Get the core logger instance for logging messages

    static Shared<spdlog::logger> Get();
    static Shared<spdlog::logger> Get(const std::string &name);

    static void SetLevel(spdlog::level::level_enum level);

private:
    static Shared<spdlog::logger> s_CoreLogger;
};

} // namespace RS

// ============================================================================
// LOGGING MACROS
// ============================================================================

#if defined(RS_ENABLE_TRACE)
    #define LOG_TRACE(...) ::RS::Logger::Get().trace(__VA_ARGS__)
#else
    #define LOG_TRACE(...)
#endif
#define LOG_DEBUG(...) ::RS::Logger::Get()->debug(__VA_ARGS__)
#define LOG_INFO(...) ::RS::Logger::Get()->info(__VA_ARGS__)
#define LOG_WARN(...) ::RS::Logger::Get()->warn(__VA_ARGS__)
#define LOG_ERROR(...) ::RS::Logger::Get()->error(__VA_ARGS__)
#define LOG_CRITICAL(...) ::RS::Logger::Get()->critical(__VA_ARGS__)

// ============================================================================
// USAGE EXAMPLES
// ============================================================================

// LOG_INFO("Engine initialized");
// LOG_WARN("Invalid user input: {}", userValue);
// LOG_ERROR("Failed to load asset: {}", assetPath);
// LOG_CRITICAL("Unrecoverable error occurred");
