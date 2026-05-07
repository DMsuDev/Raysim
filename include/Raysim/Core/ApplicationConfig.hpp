#pragma once

#include "Raysim/Core/WindowProperties.hpp"
#include "Raysim/Core/Log.hpp"

namespace RS
{
    /**
     * @struct ApplicationConfig
     * @brief Configuration structure for initializing the Application.
     *
     * Pass an instance to the Application constructor to configure the window,
     * backends, frame rate, and logging before the main loop starts.
     * All fields have sensible defaults so partial initialization works fine.
     */
    struct ApplicationConfig
    {
        WindowProps Window = WindowProps();            // Window properties (Title, Width, Height)

        // Minimum log level shown in the console.
        // Defaults to Trace in debug builds and Info in release builds.
#ifdef RS_BUILD_DEBUG
        LogLevel ConsoleLogLevel = LogLevel::Trace;
#else
        LogLevel ConsoleLogLevel = LogLevel::Info;
#endif

        ApplicationConfig() = default;
    };

} // namespace RS
