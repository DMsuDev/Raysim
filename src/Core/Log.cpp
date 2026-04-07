#include "../pch.hpp"

#include <spdlog/sinks/stdout_color_sinks.h>
#include <spdlog/sinks/rotating_file_sink.h>
#include <spdlog/async.h>

namespace RS {

//==============================================================================
// Static state
//==============================================================================

Shared<spdlog::logger> Log::s_CoreLogger   = nullptr;
Shared<spdlog::logger> Log::s_ClientLogger = nullptr;

//==============================================================================
// Initialization
//==============================================================================

void Log::Init(bool async)
{
    if (IsInitialized())
        return;

    try {
        // ---- Shared sinks (used by both loggers) ----------------------------

        auto consoleSink = std::make_shared<spdlog::sinks::stdout_color_sink_mt>();
        consoleSink->set_level(spdlog::level::trace);
        /// Console pattern: colored `[HH:MM:SS] [logger] message`.
        consoleSink->set_pattern("%^[%T] [%n] %v%$");

        auto fileSink = std::make_shared<spdlog::sinks::rotating_file_sink_mt>(
            "Raysim.log",
            1024 * 1024 * 10,  // 10 MB per file
            5,                 // keep 5 rotated files
            false
        );
        fileSink->set_level(spdlog::level::trace);
        /// File pattern: `[HH:MM:SS] [level] logger: message`.
        fileSink->set_pattern("[%T] [%l] %n: %v");

        std::vector<spdlog::sink_ptr> sinks{ consoleSink, fileSink };

        // ---- Create core & client loggers -----------------------------------

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
    spdlog::shutdown();
    s_CoreLogger = nullptr;
    s_ClientLogger = nullptr;
}

//==============================================================================
// Global configuration
//==============================================================================

void Log::SetLevel(spdlog::level::level_enum level)
{
    RS_CORE_ASSERT(IsInitialized(), "Log not initialized. Call Log::Init() first.");
    s_CoreLogger->set_level(level);
    s_ClientLogger->set_level(level);
}

void Log::SetConsoleLevel(spdlog::level::level_enum level)
{
    RS_CORE_ASSERT(IsInitialized(), "Log not initialized. Call Log::Init() first.");
    // Console sink is at index 0 in both loggers (they share sinks)
    if (!s_CoreLogger->sinks().empty())
        s_CoreLogger->sinks()[0]->set_level(level);
}

void Log::SetFileLevel(spdlog::level::level_enum level)
{
    RS_CORE_ASSERT(IsInitialized(), "Log not initialized. Call Log::Init() first.");
    // File sink is at index 1 in both loggers (they share sinks)
    if (s_CoreLogger->sinks().size() > 1)
        s_CoreLogger->sinks()[1]->set_level(level);
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

void Log::SetClientLevel(spdlog::level::level_enum level)
{
    RS_CORE_ASSERT(IsInitialized(), "Log not initialized. Call Log::Init() first.");
    s_ClientLogger->set_level(level);
}

} // namespace RS
