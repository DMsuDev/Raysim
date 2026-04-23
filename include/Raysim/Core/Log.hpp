/**
 * @file Log.hpp
 * @brief Logging system using spdlog for both engine (core) and application (client) logging.
 *
 * This header defines the `RS::Log` class, which manages two separate loggers:
 * - Core Logger: Used internally by the engine for debugging and informational messages.
 * - Client Logger: Exposed to the application for user-defined logging.
 *
 * Both loggers can be configured independently, but share common sinks (console and file) by default.
 * The logging macros (e.g. `RS_CORE_ERROR`, `RS_ERROR`) provide convenient access to the respective loggers.
 *
 * Usage:
 * 1. Call `RS::Log::Init()` at the start of your application to set up the logging system.
 * 2. Use the provided macros to log messages at various levels (trace, debug, info, warn, error, critical).
 *
 * @note This logging system is designed to be thread-safe and efficient, leveraging spdlog's asynchronous capabilities.
 * @note The client does not need to call `Init()` on the loggers directly, the system is initialized with sensible defaults. However, you can customize the client logger or its sinks if needed.
 */

#pragma once

#include "Raysim/Core/Base.hpp"
#include <unordered_set>
#include <unordered_map>
#include <chrono>
#include <string>
#include <mutex>

// Suppress warnings from external spdlog headers
#if defined(_MSC_VER)
    #pragma warning(push, 0)
#elif defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wall"
    #pragma GCC diagnostic ignored "-Wextra"
#endif

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#if defined(_MSC_VER)
    #pragma warning(pop)
#elif defined(__GNUC__) || defined(__clang__)
    #pragma GCC diagnostic pop
#endif

namespace RS {

/// @brief Log levels for filtering messages. Maps to spdlog levels internally.
enum class LogLevel : uint8_t {
    Trace = 0,
    Debug,
    Info,
    Warn,
    Error,
    Critical
};

// Forward declaration for macro use (not for user API)
spdlog::level::level_enum ToSpd(LogLevel level);

/// @brief Registry for logging messages only once per unique key (e.g. function name or event type).
/// Useful for avoiding log spam from frequently called code paths while still capturing important events.
class LogOnceRegistry {
public:
    /**
     * @brief Log a message only once per unique key (thread-safe).
     * @param key Unique identifier for the log message (e.g. function name or event type).
     */
    static bool ShouldLog(const std::string& key) {
        static std::unordered_set<std::string> loggedKeys;
        static std::mutex mutex;

        std::lock_guard<std::mutex> lock(mutex);
        return loggedKeys.insert(key).second; // Returns true if the key was not already present
    }

    /**
     * @brief Log a message only once per unique key (thread-safe).
     * @param key Unique identifier for the log message (e.g. function name or event type).
     * @param level Log level to use for the message.
     * @param msg The message to log if this is the first time the key is seen.
     */
    static void LogOnce(const std::string& key, LogLevel level, const std::string& msg);
};

/// @brief Registry for logging messages at most once per TTL (time-to-live) duration for a unique key (thread-safe).
/// Useful for rate-limiting log messages from frequently called code paths while still capturing important events
class LogTTLRegistry {
public:
    /**
     * @brief Log a message at most once per TTL (time-to-live) duration for a unique key (thread-safe).
     * @param key Unique identifier for the log message (e.g. function name or event type).
     * @param ttlSeconds Duration in seconds that must elapse before the same message can be logged again.
     * @param level Log level to use for the message.
     * @param msg The message to log if the TTL has expired for this key.
     */
    static bool ShouldLog(const std::string& key, double ttlSeconds) {
        using clock = std::chrono::steady_clock;

        static std::unordered_map<std::string, clock::time_point> logTimestamps;
        static std::mutex mutex;

        std::lock_guard<std::mutex> lock(mutex);

        auto now = clock::now();

        auto it = logTimestamps.find(key);

        if (it == logTimestamps.end()) {
            logTimestamps[key] = now;
            return true;
        }

        double elapsed = std::chrono::duration<double>(now - it->second).count();

        if (elapsed >= ttlSeconds) {
            it->second = now;
            return true;
        }

        return false;
    }

    static void Log(const std::string& key, double ttlSeconds, LogLevel level, const std::string& msg);
};

/// @brief Static logging facade. Manages two independent loggers:
///
/// - `CORE`: internal engine logging (not intended for end-users).
///
/// - `CLIENT`: application-side logging (configurable by the user).
///
/// Call `Log::Init()` once at startup before any logging macro is used.
/// Both loggers write to the same console and file sinks by default,
/// but the client logger name and levels can be customized.
class Log
{
public:
    /// Initialize both Core and Client loggers.
    /// @param async If true, logging is asynchronous (bounded queue, non-blocking).
    static void Init(bool async = true);

    /// Clean up loggers and flush all sinks. Call on application shutdown.
    static void Shutdown();

    // ------- Accessors -------------------------------------------------------

    /// @brief Returns the core (engine) logger.
    static Shared<spdlog::logger>& GetCoreLogger()   { return s_CoreLogger; }

    /// @brief Returns the client (application) logger.
    static Shared<spdlog::logger>& GetClientLogger()  { return s_ClientLogger; }

    /// True after `Init()` has completed successfully.
    static bool IsInitialized() { return s_CoreLogger != nullptr; }

    // ------- Global configuration --------------------------------------------

    /// Set level filter on **both** loggers.
    static void SetLevel(LogLevel level);

    /// Set the minimum level for the console sink (applies to both loggers).
    static void SetConsoleLevel(LogLevel level);

    /// Set the minimum level for the file sink (applies to both loggers).
    static void SetFileLevel(LogLevel level);

    /// Force-flush all sinks immediately.
    static void Flush();

    // ------- Client logger configuration -------------------------------------

    /// Replace the client logger with a custom spdlog logger instance.
    /// Useful for integrating an external or differently-configured logger.
    /// @param logger A valid, non-null spdlog logger.
    static void SetClientLogger(Shared<spdlog::logger> logger);

    /// Change the display name of the client logger (e.g. your app name).
    /// Internally clones the current client logger with the new name.
    /// @param name New name for the client logger (appears in log messages).
    static void SetClientName(const std::string& name);

    /// Set the level filter on the client logger only.
    /// @param level Minimum log level for the client logger.
    static void SetClientLevel(LogLevel level);

private:
    static Shared<spdlog::logger> s_CoreLogger;
    static Shared<spdlog::logger> s_ClientLogger;
};

} // namespace RS

// ============================================================================
// LOGGING MACROS
// ============================================================================
// Prefixed with RS_ to avoid collisions with other libraries that
// define LOG_ERROR, LOG_INFO, etc. as plain constants/macros.

// Core log macros (engine internals)
#define RS_CORE_TRACE(...)     ::RS::Log::GetCoreLogger()->trace(__VA_ARGS__)
#define RS_CORE_DEBUG(...)     ::RS::Log::GetCoreLogger()->debug(__VA_ARGS__)
#define RS_CORE_INFO(...)      ::RS::Log::GetCoreLogger()->info(__VA_ARGS__)
#define RS_CORE_WARN(...)      ::RS::Log::GetCoreLogger()->warn(__VA_ARGS__)
#define RS_CORE_ERROR(...)     ::RS::Log::GetCoreLogger()->error(__VA_ARGS__)
#define RS_CORE_CRITICAL(...)  ::RS::Log::GetCoreLogger()->critical(__VA_ARGS__)
#define RS_CORE_LOG(level, ...) ::RS::Log::GetCoreLogger()->log(::RS::ToSpd(level), __VA_ARGS__)

// Client log macros (application code)
#define RS_TRACE(...)          ::RS::Log::GetClientLogger()->trace(__VA_ARGS__)
#define RS_DEBUG(...)          ::RS::Log::GetClientLogger()->debug(__VA_ARGS__)
#define RS_INFO(...)           ::RS::Log::GetClientLogger()->info(__VA_ARGS__)
#define RS_WARN(...)           ::RS::Log::GetClientLogger()->warn(__VA_ARGS__)
#define RS_ERROR(...)          ::RS::Log::GetClientLogger()->error(__VA_ARGS__)
#define RS_CRITICAL(...)       ::RS::Log::GetClientLogger()->critical(__VA_ARGS__)
#define RS_LOG(level, ...)     ::RS::Log::GetClientLogger()->log(::RS::ToSpd(level), __VA_ARGS__)
