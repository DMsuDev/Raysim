/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 *   Raysim is licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

#pragma once

#include <chrono>
#include <ctime>
#include <string>
#include <spdlog/fmt/chrono.h>

namespace RS::Utils {

/**
 * @namespace Time
 * @brief Utility functions for time management, formatting, and timestamps.
 */
namespace Time {

//==============================================================================
// Type aliases
//==============================================================================

/// Wall-clock: suitable for timestamps, dates, and log output.
using SystemClock = std::chrono::system_clock;

/// Monotonic clock: suitable for frame timing, profiling, and timers.
/// Not anchored to wall time — do not use for date/time display.
using SteadyClock = std::chrono::steady_clock;

using TimePoint       = SystemClock::time_point;
using SteadyTimePoint = SteadyClock::time_point;

//==============================================================================
// Current time
//==============================================================================

/// @brief Returns the current wall-clock time point.
/// Use for timestamps, log output, filenames, and date/time display.
[[nodiscard]]
inline TimePoint Now() noexcept
{
    return SystemClock::now();
}

/// @brief Returns the current monotonic time point.
/// Use for frame timing, profiling, timers, and delta time.
/// Not suitable for date/time formatting — use @ref Now() for that.
[[nodiscard]]
inline SteadyTimePoint NowSteady() noexcept
{
    return SteadyClock::now();
}

//==============================================================================
// Internal helper
//==============================================================================

namespace detail {

/// Thread-safe local time conversion. Uses localtime_s (MSVC/MinGW) or localtime_r (POSIX).
[[nodiscard]]
inline std::tm ToLocaltime(std::time_t t) noexcept
{
    std::tm tm{};
#if defined(_WIN32) || defined(__MINGW32__)
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif
    return tm;
}

[[nodiscard]]
inline std::tm NowLocal() noexcept
{
    return ToLocaltime(SystemClock::to_time_t(Now()));
}

} // namespace detail

//==============================================================================
// Timestamp formatting (local time)
//==============================================================================

/// @brief Formats the current local time using a custom format string.
/// The format string uses the same syntax as std::put_time and fmt::format.
[[nodiscard]]
inline std::string FormatLocalTime(const std::string& format)
{
    return fmt::format(fmt::runtime(format), detail::NowLocal());
}

/// @brief Returns the current local date and time as a string.
/// Example: `2026-05-12 18:42:15`
[[nodiscard]]
inline std::string Timestamp()
{
    return fmt::format("{:%Y-%m-%d %H:%M:%S}", detail::NowLocal());
}

/// @brief Returns the current local date as a string.
/// Example: `2026-05-12`
[[nodiscard]]
inline std::string Date()
{
    return fmt::format("{:%Y-%m-%d}", detail::NowLocal());
}

/// @brief Returns the current local time as a string.
/// Example: `18:42:15`
[[nodiscard]]
inline std::string ClockTime()
{
    return fmt::format("{:%H:%M:%S}", detail::NowLocal());
}

/// @brief Returns the current weekday name.
/// Example: `Tuesday`
[[nodiscard]]
inline std::string Weekday()
{
    return fmt::format("{:%A}", detail::NowLocal());
}

/// @brief Returns the current month name.
/// Example: `May`
[[nodiscard]]
inline std::string Month()
{
    return fmt::format("{:%B}", detail::NowLocal());
}

/// @brief Returns the current year as a string.
/// Example: `2026`
[[nodiscard]]inline std::string Year()
{
    return fmt::format("{:%Y}", detail::NowLocal());
}

/// @brief Returns the current local date and time in a verbose format.
/// Example: `Tuesday, May 12, 2026`
[[nodiscard]]
inline std::string FullDate()
{
    return fmt::format("{:%A, %B %d, %Y}", detail::NowLocal());
}

/// @brief Returns a filename-safe timestamp string.
/// Example: `20260512_184215`
[[nodiscard]]
inline std::string FileTimestamp()
{
    return fmt::format("{:%Y%m%d_%H%M%S}", detail::NowLocal());
}

} // namespace Time

} // namespace RS::Utils
