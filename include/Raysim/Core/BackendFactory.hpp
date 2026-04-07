#pragma once

#include <memory>

#include "../Interfaces/IRenderer.hpp"
#include "../Interfaces/IWindow.hpp"
#include "Raysim/Input/Input.hpp"

namespace RS {

// ============================================================================
// BACKEND TYPE
// ============================================================================

/**
 * @enum BackendType
 * @brief Supported rendering and input backends
 *
 * Used to select the underlying platform/library that powers the window,
 * renderer, input and timing systems.
 */
enum class BackendType
{
    Raylib  // Raylib-based backend (default, cross-platform)
};

// ============================================================================
// BACKEND FACTORY
// ============================================================================

/**
 * @class BackendFactory
 * @brief Factory that creates concrete backend implementations
 *
 * Provides static methods to create instances of IRenderer, IWindow, and Input
 * based on the specified BackendType. This allows the application to remain agnostic of the underlying platform and easily switch backends if needed.
 */
class BackendFactory
{
public:
    BackendFactory() = delete; // Static-only class

    /**
     * @brief Create a renderer for the specified backend
     * @param type The backend to use
     * @return Owning pointer to a concrete IRenderer implementation
     */
    static std::unique_ptr<IRenderer> CreateRenderer(BackendType type);

    /**
     * @brief Create a window for the specified backend
     * @param type The backend to use
     * @return Owning pointer to a concrete IWindow implementation
     */
    static std::unique_ptr<IWindow> CreateWindow(BackendType type);

    /**
     * @brief Create an input handler for the specified backend
     * @param type The backend to use
     * @return Owning pointer to a concrete Input implementation
     */
    static std::unique_ptr<Input> CreateInput(BackendType type);
};

} // namespace RS
