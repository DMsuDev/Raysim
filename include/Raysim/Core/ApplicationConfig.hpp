#pragma once

#include "Raysim/Core/BackendFactory.hpp"

namespace RS {

/**
 * @struct ApplicationConfig
 * @brief Configuration structure for initializing the Application.
 *
 * Pass an instance to the Application constructor to configure the window,
 * backends, frame rate, and logging before the main loop starts.
 * All fields have sensible defaults so partial initialization works fine.
 */
struct ApplicationConfig {
    WindowProps     Window        = WindowProps();          ///< Window properties (Title, Width, Height)
    WindowBackend   Backend       = WindowBackend::Raylib;  ///< Platform backend (window & input)
    RenderAPI       Renderer      = RenderAPI::Raylib;      ///< Rendering API backend
    uint32_t        MaxFixedSteps = 8;                      ///< Max fixed update steps per frame (prevents spiral of death)

    ApplicationConfig() = default;
};

} // namespace RS
