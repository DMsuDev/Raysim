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

// Common input definitions for keyboard, mouse, and gamepad.

#include "Raysim/Input/KeyCodes.hpp"
#include "Raysim/Input/MouseCodes.hpp"
#include "Raysim/Input/GamepadCodes.hpp"

#include "Raysim/Math/Types/Vector2.hpp"

namespace RS {

class Event; // Forward declaration

/**
 * @interface Input
 * @brief Abstract interface for backend‑agnostic input handling.
 *
 * Provides a unified set of virtual methods for querying keyboard, mouse,
 * and gamepad state. Each backend implements this interface to expose a
 * consistent input API to the engine.
 *
 * The interface covers:
 * - Keyboard input: key states, modifiers, scancodes, character input.
 * - Mouse input: position, buttons, wheel, cursor mode.
 * - Gamepad input: connection state, buttons, axes.
 *
 * Backends may override Update() when explicit polling is required; otherwise,
 * it may remain a no‑op if the backend updates input state internally.
 */
class Input
{
public:
    virtual ~Input() = default;

    // ======================================================================
    // Main update method (optional override for backends that require explicit polling)
    // ======================================================================

    /// @brief Updates the input state for the current frame.
    ///
    /// Call once per frame from the main thread before reading input.
    /// Backend implementations should poll events, update states
    /// (down/pressed/released), populate the character queue, and compute deltas.
    virtual void Update() {}

    // ======================================================================
    // Event hook (event-driven backends)
    // ======================================================================

    /// @brief Feed an engine event into this input backend.
    ///
    /// Called by Application::OnEvent() for every event the Window dispatches.
    /// Polling backends (e.g. Raylib) can ignore this; event-driven backends
    /// (e.g. GLFW) use it to update their internal state arrays so that
    /// IsKeyPressed() / IsKeyDown() / etc. work correctly.
    ///
    /// @param e  The event to process (KeyPressedEvent, MouseMovedEvent, etc.).
    virtual void OnEvent([[maybe_unused]] Event& e) {}

    // ======================================================================
    // Keyboard Methods
    // ======================================================================

    /// Returns true if the key is currently held down.
    virtual bool IsKeyDown(KeyCode key) const = 0;

    /// Returns true if the key was pressed this frame.
    virtual bool IsKeyPressed(KeyCode key) const = 0;

    /// Returns true if the key was released this frame.
    virtual bool IsKeyReleased(KeyCode key) const = 0;

    /// Returns true if the key is held and auto-repeating.
    virtual bool IsKeyRepeating(KeyCode key) const = 0;

    /// Returns the last key pressed this frame, or KeyCode::Unknown if none.
    virtual KeyCode GetLastKeyPressed() const = 0;

    /// @brief Returns the next Unicode codepoint from the character input queue.
    ///
    /// Each call drains one character. Returns 0 when the queue is empty.
    /// Suitable for text-input fields that need the actual typed character,
    /// independent of keyboard layout or key combinations (e.g. accented letters,
    /// composed characters, dead-key sequences).
    ///
    /// Call in a loop to drain all characters produced this frame:
    /// @code
    ///   uint32_t cp;
    ///   while ((cp = input.GetCharPressed()) != 0) { /* use cp */ }
    /// @endcode
    virtual uint32_t GetCharPressed() { return 0; }

    /// Translates a KeyCode to a backend-specific physical key identifier.
    virtual ScanCode GetScanCode(KeyCode key) const = 0;

    // ======================================================================
    // Additional Keyboard Methods
    // ======================================================================

    /// @brief Translates a backend-specific physical key identifier back to a KeyCode.
    ///
    /// This allows reverse mapping from scancodes to the engine's abstract key codes.
    ///
    /// @param sc The scan code to translate (backend-specific).
    ///
    /// @return The corresponding `KeyCode`, or `KeyCode::Unknown` if the scan code is unmapped.
    virtual KeyCode GetKeyFromScanCode(ScanCode sc) const = 0;

    /// @brief Retrieves the current state of modifier keys as a bitmask.
    /// The returned `Modifier` value may contain multiple flags (e.g. Shift + Control).
    virtual Modifier GetModifiers() const = 0;

    // ======================================================================
    // Mouse Methods
    // ======================================================================

    /// @brief Returns the cursor position in window (client) coordinates.
    ///
    /// Coordinates are in pixels with origin at the top-left of the window.
    /// @return `Math::Vec2f` containing the x and y coordinates of the mouse cursor.
    virtual Math::Vec2f GetMousePosition() const = 0;

    /// Gets the mouse movement delta since the last frame.
    /// @return `Math::Vec2f` representing the change in mouse position (deltaX, deltaY) in pixels.
    virtual Math::Vec2f GetMouseDelta() const = 0;

    /// Gets the mouse wheel movement for the current frame.
    /// @return A float representing the amount of wheel movement (positive for forward/up, negative for backward/down).
    virtual float GetMouseWheelMove() const = 0;

    /// Checks if the specified mouse button is currently held down.
    virtual bool IsMouseButtonDown(MouseButton button) const = 0;

    /// Checks if the specified mouse button was pressed this frame.
    virtual bool IsMouseButtonPressed(MouseButton button) const = 0;

    /// Checks if the specified mouse button was released this frame.
    virtual bool IsMouseButtonReleased(MouseButton button) const = 0;

    // ======================================================================
    // Cursor Methods
    // ======================================================================

    /// Sets the cursor position within the application window.
    ///
    /// @param x X coordinate in pixels (origin at top-left of the window).
    /// @param y Y coordinate in pixels (origin at top-left of the window).
    virtual void SetCursorPosition(float x, float y) = 0;

    /// Sets the cursor visibility.
    /// @param visible True to make the cursor visible, false to hide it.
    virtual void SetCursorVisible(bool visible) = 0;

    /// Sets the cursor mode, affecting visibility and behavior.
    virtual void SetCursorMode(CursorMode mode) = 0;

    /// Returns the current cursor mode (normal, hidden, locked).
    virtual CursorMode GetCursorMode() const = 0;

    // ======================================================================
    // Gamepad Input
    // ======================================================================

    /// Checks whether the specified gamepad is currently recognized and active.
    ///
    /// @param gamepad The index of the gamepad to query (0-based).
    virtual bool IsGamepadAvailable(int gamepad) const = 0;

    /// Checks whether the given button is currently held down.
    ///
    /// @param gamepad The index of the gamepad to query.
    /// @param button The specific button to check (e.g. GamepadButton::A).
    virtual bool IsGamepadButtonDown(int gamepad, GamepadButton button) const = 0;

    /// Checks whether the given button was pressed this frame.
    ///
    /// @param gamepad The index of the gamepad to query.
    /// @param button The specific button to check (e.g. GamepadButton::A).
    virtual bool IsGamepadButtonPressed(int gamepad, GamepadButton button) const = 0;

    /// Checks whether the given button was released this frame.
    ///
    /// @param gamepad The index of the gamepad to query.
    /// @param button The specific button to check (e.g. GamepadButton::A).
    virtual bool IsGamepadButtonReleased(int gamepad, GamepadButton button) const = 0;

    /// Retrieves the normalized axis value in the range [-1.0, 1.0].
    ///
    /// @param gamepad The index of the gamepad to query.
    /// @param axis The specific axis to retrieve (e.g. GamepadAxis::LeftX).
    virtual float GetGamepadAxisValue(int gamepad, GamepadAxis axis) const = 0;

    /// @brief Retrieves the normalized axis value for a custom axis index.
    ///
    /// This method allows querying axes that may not be covered by the GamepadAxis enum,
    /// such as additional triggers, D-pad axes, or vendor-specific controls.
    ///
    /// @param gamepad The index of the gamepad to query.
    /// @param axis The index of the axis to retrieve (backend-specific mapping).
    ///
    /// @return The normalized axis value in the range [-1.0, 1.0], or 0.0 if the axis index is invalid.
    virtual float GetGamepadAxisValue(int gamepad, int axis) const = 0;

    /// @brief Returns the human-readable name reported by the backend for the given gamepad.
    ///
    /// Useful for UI labels (e.g. "Xbox 360 Controller", "DualSense").
    /// Returns an empty string if the gamepad is not available or the backend
    /// does not expose device names.
    ///
    /// @param gamepad The index of the gamepad to query (0-based).
    virtual const char* GetGamepadName(int gamepad) const { (void)gamepad; return ""; }
};

} // namespace RS
