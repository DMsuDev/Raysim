#pragma once

#include <string>

namespace RS {

class Event; // Forward declaration of Event class

/**
 * @brief Abstract base class representing an application "layer".
 *
 * A Layer is a modular unit that receives lifecycle callbacks, can handle
 * events, perform per-frame updates, and render ImGui-based UI. Typical uses
 * include separating systems such as input handling, game logic, and
 * debugging overlays.
 */
class Layer {
public:
    Layer(const std::string& name = "Layer") : m_DebugName(name) {}
    virtual ~Layer() = default;

// ========================================================================
// Stack Lifecycle Callbacks
// ========================================================================

    /**
     * @brief Called when the layer is attached to the Layer stack.
     *
     * This callback is used to initialize resources, register callbacks, and
     * perform setup that complements what will be cleaned up in OnDetach().
     * It is invoked when the layer is added to the stack.
     */
    virtual void OnAttach() {}

    /**
     * @brief Called when the layer is detached from the Layer stack.
     *
     * This callback is used to release resources, unregister callbacks, and
     * perform cleanup that complements what was done in OnAttach(). It is
     * invoked when the layer is removed from the stack or before destruction.
     */
    virtual void OnDetach() {}

// ========================================================================
// Event Callback
// ========================================================================

    /**
     * @brief Called when an event is dispatched to this layer.
     *
     * Events propagate through the LayerStack in reverse order (top-most layer
     * first). A layer can stop propagation by setting `e.Handled = true` inside
     * this callback (or by returning true from an EventDispatcher::Dispatch call,
     * which sets Handled automatically).
     *
     * @param e  The event to process. Cast with EventDispatcher for type safety.
     */
    virtual void OnEvent([[maybe_unused]] Event& e) {}

// ========================================================================
// Update and Render Callbacks
// ========================================================================

    /**
     * @brief Called every frame to update the layer.
     *
     * @param dt Delta time since the last frame in seconds.
     */
    virtual void OnUpdate([[maybe_unused]] float dt) {}

    /**
     * @brief ImGui rendering callback invoked every frame.
     *
     * ImGui widgets and windows are recorded in this callback. This is intended
     * solely for UI/debug rendering and should not be used for heavy CPU/GPU
     * work. This callback is typically executed before other rendering so that
     * UI appears on top.
     */
    virtual void OnUIRender() {}

// ========================================================================
// Getters
// ========================================================================

    /**
     * @brief Retrieves the debug/display name of this layer.
     *
     * The returned reference remains valid while the layer instance exists.
     *
     * @return const std::string& The layer's debug/display name.
     */
    [[nodiscard]] const std::string& GetName() const { return m_DebugName; }

private:

    /// Layer debug/display name (useful for debugging and UI)
    std::string m_DebugName;
};

} // namespace RS
