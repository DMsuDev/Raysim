#pragma once

#include "Raysim/Core/WindowProperties.hpp"

namespace RS
{
    // Forward declarations of core engine subsystems to avoid circular dependencies
    class RendererAPI;
    class Window;
    class Input;
    class ImGuiBackend;
    namespace Fonts { class FontRenderer; }

    /**
     * @class BackendFactory
     * @brief Factory that creates concrete backend implementations.
     *
     * The backend is selected at CMake configure time via -DRS_BACKEND=<name>,
     * which sets a preprocessor flag (e.g. RS_BACKEND_RAYLIB). All methods
     * dispatch at compile time using #if defined(...) blocks - there is no
     * runtime selection.
     *
     * @see RS_BACKEND_RAYLIB, RS_BACKEND_GLFW_OPENGL
     */
    class BackendFactory
    {
    public:
        BackendFactory() = delete; // Static-only class

        /**
         * @brief Create the renderer for the configured backend.
         * @return Owning pointer to a concrete RendererAPI, or nullptr if no
         *         backend flag is defined (configuration error).
         */
        static Scope<RendererAPI> CreateRenderer();

        /**
         * @brief Create the application window for the configured backend.
         * @param props Window creation parameters (title, size, etc.).
         * @return Owning pointer to a concrete Window, or nullptr if no
         *         backend flag is defined (configuration error).
         */
        static Scope<Window> CreateAppWindow(const WindowProps &props);

        /**
         * @brief Create the input handler for the configured backend.
         * @return Owning pointer to a concrete Input, or nullptr if no
         *         backend flag is defined (configuration error).
         */
        static Scope<Input> CreateInput();

        /**
         * @brief Create the font renderer for the configured backend.
         * @return Owning pointer to a concrete FontRenderer, or nullptr if no
         *         backend flag is defined (configuration error).
         */
        static Scope<Fonts::FontRenderer> CreateFontRenderer();

        /**
         * @brief Create the ImGui platform+renderer backend for the configured backend.
         * @return Owning pointer to a concrete ImGuiBackend, or nullptr if no
         *         backend flag is defined (configuration error).
         */
        static Scope<ImGuiBackend> CreateImGuiBackend();
    };

} // namespace RS
