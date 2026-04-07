#pragma once

#include "Raysim/Core/ApplicationConfig.hpp"
#include "Raysim/Core/Window.hpp"

#include "Raysim/Renderer/RenderCommand.hpp"

#include "Raysim/Input/Input.hpp"

namespace RS {

/**
 * @struct EngineContext
 * @brief A simple struct to hold pointers to core engine subsystems.
 *
 * This is passed to each Scene so they can access the Window, Renderer, and
 * Input systems without needing global singletons. It is owned and managed by
 * the Application, which populates it after creating the backends.
 */
struct EngineContext
{
    ApplicationConfig* Config    = nullptr;  ///< Configuration settings (from Application)
    RenderCommand*     Renderer  = nullptr;  ///< Command dispatcher for rendering
    Window*            Window    = nullptr;  ///< Main window (from BackendFactory)
    Input*             Input     = nullptr;  ///< Input polling (from BackendFactory)
};

} // namespace RS
