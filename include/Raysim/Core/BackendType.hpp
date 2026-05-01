#pragma once

namespace RS
{
    /**
     * @enum WindowBackend
     * @brief Identifies the windowing library to use for the application.
     *
     * The window backend provides window creation, event handling, and input processing.
     * Some backends (e.g. Raylib) integrate both windowing and input handling,
     * while others (e.g. GLFW, SDL) may require separate input handling.
     */
    enum class WindowBackend
    {
        GLFW,
        SDL,
        Raylib
    };

    /**
     * @enum RenderAPI
     * @brief Identifies the graphics rendering API to use.
     *
     * The rendering API backend is responsible for GPU operations, including drawing,
     * resource management, and synchronization.
     */
    enum class RenderAPI
    {
        OpenGL,
        Vulkan,
        DirectX12,
        Raylib,
        Null // No-op renderer
    };

} // namespace RS
