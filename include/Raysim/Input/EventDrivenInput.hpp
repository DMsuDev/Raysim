/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 *   Raysim is licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

#pragma once

#include "Raysim/Input/Input.hpp"
#include "Raysim/Events/Events.hpp"

#include <array>
#include <queue>
#include <cstdint>

namespace RS {

// ============================================================================
// EventDrivenInput
// ============================================================================

/**
 * @class EventDrivenInput
 * @brief Abstract intermediate Input implementation for event-driven backends.
 *
 * Maintains internal state arrays (down / pressed / released / repeating) for
 * keyboard and mouse, populated through OnEvent(). Backends that receive input
 * via OS callbacks (GLFW, SDL2, Win32 raw input, etc.) inherit from this class
 * instead of directly from Input.
 *
 * Polling backends (e.g. Raylib) continue to inherit from Input directly and
 * delegate to the backend's own state queries.
 *
 * ## Frame semantics
 *
 * | State     | Set when                         | Cleared when  |
 * |-----------|----------------------------------|---------------|
 * | Down      | KeyPressed event arrives         | KeyReleased   |
 * | Pressed   | KeyPressed arrives (this frame)  | Update()      |
 * | Released  | KeyReleased arrives (this frame) | Update()      |
 * | Repeating | KeyPressed with isRepeat==true   | Update()      |
 *
 * Update() **must** be called once per frame (before scene/layer Update) to
 * clear the per-frame transient states.
 *
 * ## Implementing a new backend
 *
 * 1. Inherit from EventDrivenInput.
 * 2. In your constructor, register OS/library callbacks that call Application's
 *    window event callback - the rest flows automatically through OnEvent().
 * 3. Implement the remaining pure-virtual methods:
 *    - GetScanCode() / GetKeyFromScanCode()    (key map)
 *    - SetCursorPosition() / SetCursorVisible() / SetCursorMode()  (cursor API)
 *    - Gamepad methods (backend-specific)
 */
class EventDrivenInput : public Input
{
public:
    // ======================================================================
    // Lifecycle
    // ======================================================================

    EventDrivenInput()  = default;
    ~EventDrivenInput() = default;

    /**
     * @brief Flush per-frame transient states (Pressed, Released, Repeating).
     *
     * Must be called once per frame, typically from the main Application loop
     * before scene update. Missing this call causes Pressed/Released to remain
     * true across multiple frames.
     */
    void Update() override;

    /**
     * @brief Feed an engine event into the state arrays.
     *
     * Called by Application::OnEvent() for every event the Window dispatches.
     * Handles: KeyPressedEvent, KeyReleasedEvent, KeyTypedEvent,
     * MouseButtonPressedEvent, MouseButtonReleasedEvent,
     * MouseMovedEvent, MouseScrolledEvent.
     */
    void OnEvent(Event& e) override;

    // ======================================================================
    // Keyboard - read from internal state arrays (O(1))
    // ======================================================================

    [[nodiscard]] bool IsKeyDown(KeyCode key)      const override;
    [[nodiscard]] bool IsKeyPressed(KeyCode key)   const override;
    [[nodiscard]] bool IsKeyReleased(KeyCode key)  const override;
    [[nodiscard]] bool IsKeyRepeating(KeyCode key) const override;

    [[nodiscard]] KeyCode  GetLastKeyPressed() const override;
    [[nodiscard]] Modifier GetModifiers()      const override;

    /// Returns the next Unicode codepoint from the internal char queue, or 0 if empty.
    /// The queue is populated by KeyTypedEvent events dispatched by the window backend.
    uint32_t GetCharPressed() override;

    // ======================================================================
    // Mouse - read from internal state / position tracking
    // ======================================================================

    [[nodiscard]] Math::Vec2f GetMousePosition() const override;
    [[nodiscard]] Math::Vec2f GetMouseDelta()    const override;
    [[nodiscard]] float       GetMouseWheelMove() const override;

    [[nodiscard]] bool IsMouseButtonDown(MouseButton button)     const override;
    [[nodiscard]] bool IsMouseButtonPressed(MouseButton button)  const override;
    [[nodiscard]] bool IsMouseButtonReleased(MouseButton button) const override;

    [[nodiscard]] CursorMode  GetCursorMode() const override { return m_CursorMode; }

    // ======================================================================
    // Pure-virtual - backend-specific, deferred to derived class
    // ======================================================================

    // Scan codes are backend-specific (key map lives in the concrete backend).
    [[nodiscard]] ScanCode GetScanCode(KeyCode key)        const override = 0;
    [[nodiscard]] KeyCode  GetKeyFromScanCode(ScanCode sc) const override = 0;

    // Cursor manipulation requires native window handles.
    void SetCursorPosition(float x, float y) override = 0;
    void SetCursorVisible(bool visible)      override = 0;
    void SetCursorMode(CursorMode mode)      override = 0;

    // Gamepad access is entirely backend-specific.
    [[nodiscard]] bool  IsGamepadAvailable(int gamepad)                               const override = 0;
    [[nodiscard]] bool  IsGamepadButtonDown(int gamepad, GamepadButton button)        const override = 0;
    [[nodiscard]] bool  IsGamepadButtonPressed(int gamepad, GamepadButton button)     const override = 0;
    [[nodiscard]] bool  IsGamepadButtonReleased(int gamepad, GamepadButton button)    const override = 0;
    [[nodiscard]] float GetGamepadAxisValue(int gamepad, GamepadAxis axis)            const override = 0;
    [[nodiscard]] float GetGamepadAxisValue(int gamepad, int axis)                    const override = 0;

protected:

    // ======================================================================
    // Internal event handlers
    // ======================================================================

    void HandleKeyPressed(const KeyPressedEvent& e);
    void HandleKeyReleased(const KeyReleasedEvent& e);
    void HandleKeyTyped(const KeyTypedEvent& e);
    void HandleMouseButtonPressed(const MouseButtonPressedEvent& e);
    void HandleMouseButtonReleased(const MouseButtonReleasedEvent& e);
    void HandleMouseMoved(const MouseMovedEvent& e);
    void HandleMouseScrolled(const MouseScrolledEvent& e);

    // ======================================================================
    // Keyboard state
    // ======================================================================

    static constexpr std::size_t KeyCount = static_cast<std::size_t>(KeyCode::Count);

    std::array<bool, KeyCount> m_KeyDown      {};   ///< Currently held
    std::array<bool, KeyCount> m_KeyPressed   {};   ///< Pressed this frame (cleared by Update)
    std::array<bool, KeyCount> m_KeyReleased  {};   ///< Released this frame (cleared by Update)
    std::array<bool, KeyCount> m_KeyRepeating {};   ///< Auto-repeat this frame (cleared by Update)

    KeyCode  m_LastKeyPressed = KeyCode::Unknown;
    Modifier m_Modifiers      = Modifier::None;

    // Unicode codepoints received via KeyTypedEvent, drained one-by-one by GetCharPressed().
    std::queue<uint32_t> m_CharQueue;

    // ======================================================================
    // Mouse state
    // ======================================================================

    static constexpr std::size_t ButtonCount = MouseButtonCount;

    std::array<bool, ButtonCount> m_MouseDown     {};
    std::array<bool, ButtonCount> m_MousePressed  {};
    std::array<bool, ButtonCount> m_MouseReleased {};

    Math::Vec2f m_MousePosition {};
    Math::Vec2f m_MouseDelta    {};
    float       m_ScrollY       = 0.0f; ///< Vertical wheel (Y-axis dominates for GetMouseWheelMove)

    CursorMode m_CursorMode = CursorMode::Normal;
};

} // namespace RS
