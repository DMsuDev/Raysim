#pragma once

#include <string>
#include <cstdint>

namespace RS {

/**
 * @struct ApplicationConfig
 * @brief Top-level configuration for an RS::Application.
 *
 * Pass an instance to the Application constructor to configure the window,
 * frame rate, and logging before the main loop starts.
 * All fields have sensible defaults so partial initialization works fine.
 */
struct ApplicationConfig {
    std::string title     = "Raysim App";  ///< Window title
    uint32_t    width     = 1600;          ///< Window width in pixels
    uint32_t    height    = 900;           ///< Window height in pixels
    uint32_t    maxFixedSteps = 8;         ///< Max fixed update steps per frame (prevents spiral of death)
    std::string logFile   = "App.log";     ///< Log file name written next to the executable
};

} // namespace RS
