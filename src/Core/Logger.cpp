#include "../pch.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

namespace RS {

//==============================================================================
// Static state
//==============================================================================

Shared<spdlog::logger> Logger::s_CoreLogger = nullptr;

//==============================================================================
// Initialization
//==============================================================================

void Logger::Init(const std::string& app_name,
                  const std::string& log_file,
                  bool async,
                  spdlog::level::level_enum console_level,
                  spdlog::level::level_enum file_level)
{
    if (IsInitialized())
        return;

    try {
        std::vector<spdlog::sink_ptr> logSinks;

        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(console_level);
        logSinks.emplace_back(console_sink);

        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            log_file,
            1024 * 1024 * 10,
            5,
            false
        );
        file_sink->set_level(file_level);
        logSinks.emplace_back(file_sink);

        /// Console sink pattern: colored `[HH:MM:SS] [logger] message`.
        logSinks[0]->set_pattern("%^[%T] [%n] %v%$");

        /// File sink pattern: `[HH:MM:SS] [level] logger: message`.
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

        if (async)
        {
            /// Async logging uses a bounded queue to avoid unbounded memory usage.
            spdlog::init_thread_pool(8192, 1);

            s_CoreLogger = std::make_shared<spdlog::async_logger>(
                app_name,
                logSinks.begin(),
                logSinks.end(),
                spdlog::thread_pool(),
                spdlog::async_overflow_policy::block);
        }
        else
        {
            s_CoreLogger = std::make_shared<spdlog::logger>(
                app_name,
                logSinks.begin(),
                logSinks.end()
            );
        }

        s_CoreLogger->set_level(spdlog::level::trace); // Capture all levels at logger level
        s_CoreLogger->flush_on(spdlog::level::warn);   // Flush on warn level or higher (more responsive)

        spdlog::register_logger(s_CoreLogger);
        spdlog::set_default_logger(s_CoreLogger);

    } catch (const spdlog::spdlog_ex& ex) {
        std::fprintf(stderr, "Log init failed: %s\n", ex.what());
    }
}

//==============================================================================
// Logger access
//==============================================================================

Shared<spdlog::logger> Logger::Get()
{
    assert(s_CoreLogger && "Logger not initialized. Call Logger::Init() first.");
    return s_CoreLogger;
}

Shared<spdlog::logger> Logger::Get(const std::string& name)
{
    assert(s_CoreLogger && "Logger not initialized. Call Logger::Init() first.");

    auto existing = spdlog::get(name);
    if (existing)
        return existing;

    auto new_logger = s_CoreLogger->clone(name);
    spdlog::register_logger(new_logger);
    return new_logger;
}

//==============================================================================
// Configuration
//==============================================================================

void Logger::SetLevel(spdlog::level::level_enum level)
{
    assert(s_CoreLogger && "Logger not initialized. Call Logger::Init() first.");
    s_CoreLogger->set_level(level);
}

void Logger::SetConsoleLevel(spdlog::level::level_enum level)
{
    assert(s_CoreLogger && "Logger not initialized. Call Logger::Init() first.");
    if (s_CoreLogger->sinks().size() > 0) {
        s_CoreLogger->sinks()[0]->set_level(level);
    }
}

void Logger::SetFileLevel(spdlog::level::level_enum level)
{
    assert(s_CoreLogger && "Logger not initialized. Call Logger::Init() first.");
    if (s_CoreLogger->sinks().size() > 1) {
        s_CoreLogger->sinks()[1]->set_level(level);
    }
}

} // namespace RS
