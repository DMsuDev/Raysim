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
        WindowProps Window = WindowProps();   // Window properties (Title, Width, Height)
        int         TargetFPS = 60;           // Desired frame rate (0 for uncapped)
        bool        VSync = true;             // Whether to enable vertical sync

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
