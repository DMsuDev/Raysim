#include "rspch.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/sinks/basic_file_sink.h>
#include <spdlog/async.h>

namespace RS {

//==============================================================================
// Helpers
//==============================================================================

inline spdlog::level::level_enum ToSpd(LogLevel level)
{
    switch (level)
    {
        case LogLevel::Trace:    return spdlog::level::trace;
        case LogLevel::Debug:    return spdlog::level::debug;
        case LogLevel::Info:     return spdlog::level::info;
        case LogLevel::Warn:     return spdlog::level::warn;
        case LogLevel::Error:    return spdlog::level::err;
        case LogLevel::Critical: return spdlog::level::critical;
        default:                 return spdlog::level::info;
    }
}

static void LogWithLevel(LogLevel level, const std::string& msg) {
    switch (level) {
        case LogLevel::Trace:    RS_CORE_TRACE(msg); break;
        case LogLevel::Debug:    RS_CORE_DEBUG(msg); break;
        case LogLevel::Info:     RS_CORE_INFO(msg); break;
        case LogLevel::Warn:     RS_CORE_WARN(msg); break;
        case LogLevel::Error:    RS_CORE_ERROR(msg); break;
        case LogLevel::Critical: RS_CORE_CRITICAL(msg); break;
        default:                 RS_CORE_INFO(msg); break;
    }
}

//==============================================================================
// Static state
//==============================================================================

Shared<spdlog::logger> Log::s_CoreLogger   = nullptr;
Shared<spdlog::logger> Log::s_ClientLogger = nullptr;

//==============================================================================
// Log Registry
//==============================================================================

void LogOnceRegistry::LogOnce(const std::string& key, LogLevel level, const std::string& msg) {
    if (ShouldLog(key)) {
        LogWithLevel(level, msg);
    }
}

void LogTTLRegistry::LogTTL(const std::string& key, double ttlSeconds, LogLevel level, const std::string& msg) {
    if (ShouldLog(key, ttlSeconds)) {
        LogWithLevel(level, msg);
    }
}

//==============================================================================
// Initialization
//==============================================================================

void Log::Init(bool async, LogFileMode fileMode)
{
    if (IsInitialized())
        return;

    try {
        // Shared sinks (used by both loggers)

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        // Trace in debug builds, Info in release
#ifdef RS_BUILD_DEBUG
        consoleSink->set_level(spdlog::level::trace);
#else
        consoleSink->set_level(spdlog::level::info);
#endif
        /// Console pattern: colored `[HH:MM:SS] [logger] message`.
        consoleSink->set_pattern("%^[%T] [%n] %v%$");

        // Truncate: overwrite the existing file on each startup (basic_file_sink, truncate=true).
        // Append:   keep previous logs and rotate when the file exceeds 10 MB (rotating_file_sink).
        spdlog::sink_ptr fileSink;
        if (fileMode == LogFileMode::Truncate)
        {
            fileSink = std::make_shared<spdlog::sinks::basic_file_sink_mt>(
                "Raysim.log",
                true  // truncate=true: replaces the file instead of appending
            );
        }
        else
        {
            fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                "Raysim.log",
                1024 * 1024 * 10,  // 10 MB per file
                5                  // keep 5 rotated files
            );
        }
        fileSink->set_level(spdlog::level::trace);
        /// File pattern: `[HH:MM:SS] [level] logger: message`.
        fileSink->set_pattern("[%T] [%l] %n: %v");

        std::vector<spdlog::sink_ptr> sinks{ consoleSink, fileSink };

        // Create core and client loggers

        auto createLogger = [&](const std::string& name) -> Shared<spdlog::logger>
        {
            Shared<spdlog::logger> logger;
            if (async)
            {
                // Ensure the thread pool is initialized before creating async loggers
                if (!spdlog::thread_pool()) {
                    spdlog::init_thread_pool(8192, 1);
                }
                logger = std::make_shared<spdlog::async_logger>(
                    name, sinks.begin(), sinks.end(),
                    spdlog::thread_pool(),
                    spdlog::async_overflow_policy::block);
            }
            else
            {
                logger = std::make_shared<spdlog::logger>(
                    name, sinks.begin(), sinks.end());
            }
            logger->set_level(spdlog::level::trace);
            logger->flush_on(spdlog::level::warn);
            spdlog::register_logger(logger);
            return logger;
        };

        s_CoreLogger   = createLogger("RAYSIM");
        s_ClientLogger = createLogger("APP");

        spdlog::set_default_logger(s_CoreLogger);

    } catch (const spdlog::spdlog_ex& ex) {
        std::fprintf(stderr, "Log init failed: %s\n", ex.what());
    }
}

void Log::Shutdown()
{
    if (!IsInitialized()) return;

    s_CoreLogger.reset();
    s_ClientLogger.reset();
    spdlog::shutdown();
}

//==============================================================================
// Global configuration
//==============================================================================

void Log::SetLevel(LogLevel level)
{
    RS_CORE_ASSERT(IsInitialized(), "Log not initialized. Call Log::Init() first.");
    s_CoreLogger->set_level(ToSpd(level));
    s_ClientLogger->set_level(ToSpd(level));
}

void Log::SetConsoleLevel(LogLevel level)
{
    RS_CORE_ASSERT(IsInitialized(), "Log not initialized. Call Log::Init() first.");
    // Console sink is at index 0 in both loggers (they share sinks)
    if (!s_CoreLogger->sinks().empty())
        s_CoreLogger->sinks()[0]->set_level(ToSpd(level));
}

void Log::SetFileLevel(LogLevel level)
{
    RS_CORE_ASSERT(IsInitialized(), "Log not initialized. Call Log::Init() first.");
    // File sink is at index 1 in both loggers (they share sinks)
    if (s_CoreLogger->sinks().size() > 1)
        s_CoreLogger->sinks()[1]->set_level(ToSpd(level));
}

void Log::Flush()
{
    if (!IsInitialized()) return;
    s_CoreLogger->flush();
    s_ClientLogger->flush();
}

//==============================================================================
// Client logger configuration
//==============================================================================

void Log::SetClientLogger(Shared<spdlog::logger> logger)
{
    RS_CORE_ASSERT(logger != nullptr, "Cannot set a null client logger.");
    s_ClientLogger = std::move(logger);
}

void Log::SetClientName(const std::string& name)
{
    RS_CORE_ASSERT(IsInitialized(), "Log not initialized. Call Log::Init() first.");
    auto newLogger = s_ClientLogger->clone(name);
    spdlog::register_logger(newLogger);
    s_ClientLogger = newLogger;
}

void Log::SetClientLevel(LogLevel level)
{
    RS_CORE_ASSERT(IsInitialized(), "Log not initialized. Call Log::Init() first.");
    s_ClientLogger->set_level(ToSpd(level));
}

} // namespace RS
