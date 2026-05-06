#pragma once

#include "Raysim/Core/Base.hpp"
#include "Raysim/Core/Layer.hpp"

#include "ImGuiBackend.hpp"

#include "imgui.h"
#include "ImGuiStyles.hpp"

namespace RS {

class Window;

/**
 * @class ImGuiLayer
 * @brief Backend-agnostic layer that manages the full Dear ImGui lifecycle.
 *
 * ImGuiLayer owns an ImGuiBackend that handles the platform- and
 * renderer-specific setup (GLFW+OpenGL3, SDL+OpenGL3, Raylib, …).
 * Push it as an overlay onto a LayerStack; call Begin() / End() around
 * your per-frame ImGui widget code.
 *
 * Typical usage inside a Scene::OnAttach():
 * @code
 *   m_ImGuiLayer = new RS::ImGuiLayer(
 *       GetWindow(),
 *       RS::BackendFactory::CreateImGuiBackend(
 *           m_Context->Config->Backend,
 *           m_Context->Config->Renderer));
 *   m_LayerStack.PushOverlay(m_ImGuiLayer);
 * @endcode
 */
class ImGuiLayer : public Layer {
public:
    /**
     * @brief Construct an ImGuiLayer with an explicit backend.
     *
     * @param window  The application's main window.
     * @param backend Platform+renderer backend created via
     *                BackendFactory::CreateImGuiBackend().
     */
    explicit ImGuiLayer(Window& window, Scope<ImGuiBackend> backend);
    ~ImGuiLayer() = default;

// ========================================================================
// Layer Lifecycle Callbacks
// ========================================================================

    /**
     * @brief Called when layer is attached to the layer stack.
     *
     * Creates the ImGui context, configures IO flags, applies the default
     * style, and initializes the platform/renderer backend.
     */
    virtual void OnAttach() override;

    /**
     * @brief Called when layer is detached from the layer stack.
     *
     * Shuts down the backend and destroys the ImGui context.
     */
    virtual void OnDetach() override;

// ========================================================================
// Rendering Commands
// ========================================================================

    /**
     * @brief Begin a new ImGui frame.
     *
     * Calls the backend's BeginFrame() then ImGui::NewFrame().
     * Must be paired with a call to End() later in the same frame.
     */
    void Begin();

    /**
     * @brief End the current ImGui frame and flush draw data to the GPU.
     *
     * Updates the display size, calls ImGui::Render(), then delegates
     * to the backend's RenderDrawData().
     */
    void End();

// ========================================================================
// Configuration and utilities
// ========================================================================

    /// @brief Applies a style by name (registered in the style system).
    /// @param name The name of the style to apply (e.g. "Dark", "Light").
    /// @return true if the style exists and was applied.
    bool ApplyStyle(const std::string& name);

    /// @brief Applies a built-in style (convenience overload).
    /// @param s The built-in style to apply (e.g. BuiltinStyle::Dark).
    /// @return true if the style exists and was applied.
    bool ApplyStyle(ImGuiStyles::BuiltinStyle s);

    /// @brief Returns the name of the currently applied style.
    /// @return The name of the currently applied style.
    std::string GetCurrentStyleName() const { return m_CurrentStyle; }

    /**
     * @brief Get the ID of the currently active ImGui widget.
     *
     * This can be used to determine if a specific widget is active or focused,
     * which is useful for input handling and UI logic. The returned ID is
     * valid only during an active ImGui frame (between Begin() and End()).
     *
     * @return uint32_t The ID of the active ImGui widget, or 0 if none.
     */
    uint32_t GetActiveWidgetID() const;

private:
    Window*             m_Window      = nullptr;  ///< Non-owning reference to the main window
    Scope<ImGuiBackend> m_Backend;                ///< Owning backend (platform + renderer)
    bool                m_Initialized = false;    ///< Whether the backends are initialized
    std::string         m_CurrentStyle;           ///< Name of the currently applied style
};

} // namespace RS
