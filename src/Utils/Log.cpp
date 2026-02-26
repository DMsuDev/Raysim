#include "../pch.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

namespace RS {

// Static member definition
Shared<spdlog::logger> Logger::s_CoreLogger = nullptr;

// ========================================================================================================
// Initialization
// ========================================================================================================
void Logger::Init(const std::string& app_name,
                  const std::string& log_file,
                  bool async,
                  spdlog::level::level_enum console_level,
                  spdlog::level::level_enum file_level)
{
    if (s_CoreLogger)
        return; // Prevent double initialization
    try {
        // Create shared sinks (multiple loggers write to the same destinations)
        std::vector<spdlog::sink_ptr> logSinks;

        // Sink 1: Colored console output (for real-time monitoring)
        auto console_sink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        console_sink->set_level(console_level);
        logSinks.emplace_back(console_sink);

        // Sink 2: File output (for permanent record, auto-rotates on startup)
        auto file_sink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            log_file,
            1024 * 1024 * 10, // 10 MB
            5,                // Keep up to 5 rotated files
            false             // Do not rotate on open (append to existing file)
        );
        file_sink->set_level(file_level);
        logSinks.emplace_back(file_sink);

        // Configure console sink pattern: [HH:MM:SS] LOGGER: Message (with colors)
        logSinks[0]->set_pattern("%^[%T] [%n] %v%$");

        // Configure file sink pattern: [HH:MM:SS] [LEVEL] LOGGER: Message
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

        // Example of a more detailed pattern with thread ID and source location (uncomment if needed)
        // file_sink->set_pattern("[%Y-%m-%d %T] [%l] [%n] [thread %t] %v");

        if (async)
        {
            // Thread pool: smaller queue (1024) for more responsive flushing, 1 worker thread
            // Note: spdlog's async logger will automatically flush on certain levels (e.g., warn and above)
            // 8192 is the default queue size, but we reduce it to ensure logs are flushed more frequently, especially in a game loop context where real-time logging is important.
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
        // Fallback if logging initialization fails
        std::fprintf(stderr, "Log init failed: %s\n", ex.what());
    }
}

// ============================================================================
// Get root logger
// ============================================================================
Shared<spdlog::logger> Logger::Get()
{
    assert(s_CoreLogger && "Logger not initialized. Call Logger::Init() first.");
    return s_CoreLogger;
}

// ============================================================================
// Get or create named logger (category clone)
// ============================================================================
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

// ============================================================================
// Set global logger level (applies to all sinks)
// ============================================================================
void Logger::SetLevel(spdlog::level::level_enum level)
{
    assert(s_CoreLogger && "Logger not initialized. Call Logger::Init() first.");
    s_CoreLogger->set_level(level);
}

// ============================================================================
// Set individual sink levels
// ============================================================================
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

// ============================================================================
// Flush all sinks (force immediate write to disk/console)
// ============================================================================

void Logger::Flush()
{
    if (s_CoreLogger) {
        s_CoreLogger->flush();
    }
}

} // namespace RS
