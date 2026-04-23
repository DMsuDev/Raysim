#pragma once

#include "Raysim/Renderer/RendererAPI.hpp"
#include "Raysim/Core/Window.hpp"
#include "Raysim/Input/Input.hpp"

namespace RS {

// ============================================================================
// BACKEND ENUMS
// ============================================================================

/**
 * @enum WindowBackend
 * @brief Identifies the windowing and input library to use.
 *
 * The window backend provides window creation, event handling, and input
 * polling. Each backend also supplies its own Input implementation.
 *
 * @see BackendFactory::CreateWindow, BackendFactory::CreateInput
 */
enum class WindowBackend
{
    GLFW,       ///< GLFW windowing library (planned)
    SDL,        ///< SDL windowing library  (planned)
    Raylib      ///< Raylib: window + input integrated
};

/**
 * @enum RenderAPI
 * @brief Identifies the graphics rendering API to use.
 *
 * The render API is orthogonal to the window backend.
 * Some combinations are invalid (e.g. GLFW + Raylib renderer).
 * Use Raylib as both WindowBackend and RenderAPI when working with Raylib.
 *
 * @see BackendFactory::CreateRenderer
 */
enum class RenderAPI
{
    OpenGL,     ///< OpenGL   (placeholder: requires GLFW or SDL window)
    Vulkan,     ///< Vulkan   (placeholder: requires GLFW or SDL window)
    DirectX12,  ///< DirectX 12 (placeholder: Windows only)
    Raylib,     ///< Raylib   (uses Raylib's integrated renderer)
    Null        ///< No-op renderer: no GPU operations performed
};

// ============================================================================
// BACKEND FACTORY
// ============================================================================

/**
 * @class BackendFactory
 * @brief Factory that creates concrete backend implementations.
 *
 * Window and input are created from the WindowBackend enum (they are coupled
 * because input depends on the windowing system). The renderer is created
 * separately from the RenderAPI enum.
 *
 * @see WindowBackend, RenderAPI
 */
class BackendFactory
{
public:
    BackendFactory() = delete; // Static-only class

    /**
     * @brief Create a renderer for the specified graphics API.
     * @param api The rendering API to use.
     * @return Owning pointer to a concrete RendererAPI implementation.
     */
    static Scope<RendererAPI> CreateRenderer(RenderAPI api);

    /**
     * @brief Create a window for the specified backend.
     * @param backend The windowing library to use.
     * @return Owning pointer to a concrete Window implementation.
     */
    static Scope<Window> CreateWindow(WindowBackend backend, const WindowProps& props);

    /**
     * @brief Create an input handler for the specified backend.
     *
     * Input is tied to the windowing system, so it uses the same
     * WindowBackend enum as CreateWindow().
     *
     * @param backend The windowing library to use.
     * @return Owning pointer to a concrete Input implementation.
     */
    static Scope<Input> CreateInput(WindowBackend backend);
};

} // namespace RS
