#pragma once

#include <unordered_map>
#include <vector>

#include "Raysim/Math/Vector2.hpp"
#include "Raysim/Input/KeyCodes.hpp"

namespace RS {

/**
 * @class InputManager
 * @brief Manages keyboard, mouse, and gamepad input for the application.
 *
 * Provides easy access to input state including:
 * - Key press/release detection (using KeyCode enum)
 * - Mouse position and button state (using MouseButton enum)
 * - Gamepad input (using GamepadButton and GamepadAxis enums)
 *
 * @example
 * InputManager& input = canvas.GetInputManager();
 * if (input.IsKeyPressed(KeyCode::Space)) {
 *     // Handle space key
 * }
 * if (input.IsMouseButtonDown(MouseButton::Left)) {
 *     // Handle left mouse button
 * }
 */
class InputManager {
public:
    InputManager() = default;
    ~InputManager() = default;

    // ============================================================================
    // KEYBOARD INPUT
    // ============================================================================

    /**
     * @brief Check if a key is currently being pressed
     * @param key The key (from KeyCode enum)
     * @return true if the key is currently held down
     */
    bool IsKeyDown(KeyCode key) const;

    /**
     * @brief Check if a key was pressed this frame
     * @param key The key
     * @return true if the key was just pressed
     */
    bool IsKeyPressed(KeyCode key) const;

    /**
     * @brief Check if a key was released this frame
     * @param key The key
     * @return true if the key was just released
     */
    bool IsKeyReleased(KeyCode key) const;

    /**
     * @brief Check if a key was pressed and held for multiple frames
     * @param key The key
     * @return true if the key is held down
     */
    bool IsKeyRepeating(KeyCode key) const;

    /**
     * @brief Get the last key pressed
     * @return The key of the last pressed key, or KeyCode::Null if none
     */
    KeyCode GetLastKeyPressed() const;

    // ============================================================================
    // MOUSE INPUT
    // ============================================================================

    /**
     * @brief Get the current mouse position
     * @return Mouse position as a 2D vector
     */
    Vector2 GetMousePosition() const;

    /**
     * @brief Get the mouse position delta (movement since last frame)
     * @return Movement delta
     */
    Vector2 GetMouseDelta() const;

    /**
     * @brief Check if mouse button is currently pressed
     * @param button The mouse button (MouseButton enum)
     * @return true if the button is held down
     */
    bool IsMouseButtonDown(MouseButton button) const;

    /**
     * @brief Check if mouse button was pressed this frame
     * @param button The mouse button
     * @return true if the button was just pressed
     */
    bool IsMouseButtonPressed(MouseButton button) const;

    /**
     * @brief Check if mouse button was released this frame
     * @param button The mouse button
     * @return true if the button was just released
     */
    bool IsMouseButtonReleased(MouseButton button) const;

    /**
     * @brief Get mouse wheel movement
     * @return Scroll delta (positive = up, negative = down)
     */
    float GetMouseWheelMove() const;

    /**
     * @brief Check if mouse is within screen bounds
     * @return true if mouse is visible/on screen
     */
    bool IsMouseOnScreen() const;

    // ============================================================================
    // GAMEPAD INPUT
    // ============================================================================

    /**
     * @brief Check if a gamepad is available
     * @param gamepad The gamepad index (0-3)
     * @return true if the gamepad is connected
     */
    bool IsGamepadAvailable(int gamepad) const;

    /**
     * @brief Get gamepad button state
     * @param gamepad The gamepad index
     * @param button The button (GamepadButton enum)
     * @return true if the button is pressed
     */
    bool IsGamepadButtonDown(int gamepad, GamepadButton button) const;

    /**
     * @brief Get gamepad button pressed (just pressed this frame)
     * @param gamepad The gamepad index
     * @param button The button (GamepadButton enum)
     * @return true if the button was just pressed
     */
    bool IsGamepadButtonPressed(int gamepad, GamepadButton button) const;

    /**
     * @brief Get gamepad axis value
     * @param gamepad The gamepad index
     * @param axis The axis (GamepadAxis enum)
     * @return Axis value from -1.0 to 1.0
     */
    float GetGamepadAxisValue(int gamepad, GamepadAxis axis) const;

    /**
     * @brief Get gamepad axis value
     * @param gamepad The gamepad index
     * @param axis The axis code (Input::GAMEPAD_AXIS_LEFT_X, etc.)
     * @return Axis value from -1.0 to 1.0
     */
    float GetGamepadAxisValue(int gamepad, int axis) const;

    // ============================================================================
    // UTILITY
    // ============================================================================

    /**
     * @brief Update input state (should be called each frame)
     */
    void Update();

    /**
     * @brief Reset input state
     */
    void Reset();

private:
    Vector2 lastMousePos_ = {0, 0};
};

} // namespace RS
