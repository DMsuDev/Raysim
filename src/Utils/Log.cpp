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
                  bool async)
{
    if (s_CoreLogger)
        return; // Prevent double initialization
    try {
        // Create shared sinks (multiple loggers write to the same destinations)
        std::vector<spdlog::sink_ptr> logSinks;

        // Sink 1: Colored console output (for real-time monitoring)
        logSinks.emplace_back(
            std::make_shared<spdlog::sinks::stdout_color_sink_mt>()
        );

        // Sink 2: File output (for permanent record, auto-rotates on startup)
        logSinks.emplace_back(
            // std::make_shared<spdlog::sinks::basic_file_sink_mt>("RaySim.log", true)
            std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
                log_file,
                1024 * 1024 * 10, // 10 MB
                5,                // Keep up to 5 rotated files
                false             // Do not rotate on open (append to existing file)
            ));

        // Configure console sink pattern: [HH:MM:SS] LOGGER: Message (with colors)
        logSinks[0]->set_pattern("%^[%T] [%n] %v%$");

        // Configure file sink pattern: [HH:MM:SS] [LEVEL] LOGGER: Message
        logSinks[1]->set_pattern("[%T] [%l] %n: %v");

        // Example of a more detailed pattern with thread ID and source location (uncomment if needed)
        // file_sink->set_pattern("[%Y-%m-%d %T] [%l] [%n] [thread %t] %v");

        if (async)
        {
            // Thread pool: queue size 8192, 1 worker thread
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

        s_CoreLogger->set_level(spdlog::level::trace); // Capture all levels
        s_CoreLogger->flush_on(spdlog::level::err);    // Flush immediately (important for debugging)

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
// Set global level
// ============================================================================
void Logger::SetLevel(spdlog::level::level_enum level)
{
    assert(s_CoreLogger && "Logger not initialized. Call Logger::Init() first.");
    s_CoreLogger->set_level(level);
}

} // namespace RS
