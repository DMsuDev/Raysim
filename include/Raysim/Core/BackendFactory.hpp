#pragma once

// --- Enums ---
#include "Raysim/Core/BackendType.hpp"
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
        static Scope<Window> CreateAppWindow(WindowBackend backend, const WindowProps &props);

        /**
         * @brief Create an input handler for the specified backend.
         *
         * Input is tied to the windowing system, so it uses the same
         * WindowBackend enum as CreateAppWindow().
         *
         * @param backend The windowing library to use.
         * @return Owning pointer to a concrete Input implementation.
         */
        static Scope<Input> CreateInput(WindowBackend backend);

        /**
         * @brief Create a font renderer for the specified graphics API.
         *
         * The returned renderer is wired into FontManager via
         * FontManager::SetRenderer().
         *
         * @param api The rendering API to use.
         * @return Owning pointer to a concrete FontRenderer implementation.
         */
        static Scope<Fonts::FontRenderer> CreateFontRenderer(RenderAPI api);

        /**
         * @brief Create an ImGui platform+renderer backend.
         *
         * Returns the correct ImGuiBackend implementation for the given
         * window and renderer backends.
         *
         * @param windowBackend The windowing library in use.
         * @param renderAPI     The graphics API in use.
         * @return Owning pointer to a concrete ImGuiBackend, or nullptr on failure.
         */
        static Scope<ImGuiBackend> CreateImGuiBackend(WindowBackend windowBackend, RenderAPI renderAPI);
    };

} // namespace RS
