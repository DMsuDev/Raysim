#pragma once

#include "Raysim/Input/KeyCodes.hpp"
#include "Raysim/Input/MouseCodes.hpp"
#include "Raysim/Input/GamepadCodes.hpp"

#include "Raysim/Math/Types/Vec2.hpp"

namespace RS {

/// @interface Input
/// @brief Abstract interface for keyboard, mouse, and gamepad input.
class Input
{
public:
    virtual ~Input() = default;

    // -- Update --------------------------------------------------------------

    /// Poll or refresh input state for the current frame.
    /// Most backends refresh automatically; override only if explicit polling is needed.
    virtual void Update() {}

    // -- Keyboard ------------------------------------------------------------

    /// Returns true if the key is currently held down.
    virtual bool IsKeyDown(KeyCode key) const = 0;

    /// Returns true if the key was pressed this frame.
    virtual bool IsKeyPressed(KeyCode key) const = 0;

    /// Returns true if the key was released this frame.
    virtual bool IsKeyReleased(KeyCode key) const = 0;

    /// Returns true if the key is held and auto-repeating.
    virtual bool IsKeyRepeating(KeyCode key) const = 0;

    /// Returns the last key pressed this frame, or KeyCode::Null if none.
    virtual KeyCode GetLastKeyPressed() const = 0;

    // -- Mouse ---------------------------------------------------------------

    /// Current mouse position in screen coordinates.
    virtual Math::Vec2f GetMousePosition() const = 0;

    /// Mouse movement delta since the last frame.
    virtual Math::Vec2f GetMouseDelta() const = 0;

    /// Returns true if the mouse button is currently held down.
    virtual bool IsMouseButtonDown(MouseButton button) const = 0;

    /// Returns true if the mouse button was pressed this frame.
    virtual bool IsMouseButtonPressed(MouseButton button) const = 0;

    /// Returns true if the mouse button was released this frame.
    virtual bool IsMouseButtonReleased(MouseButton button) const = 0;

    /// Mouse wheel movement this frame (positive = up, negative = down).
    virtual float GetMouseWheelMove() const = 0;

    /// Returns true if the mouse cursor is within the window bounds.
    virtual bool IsMouseOnScreen() const = 0;

    // -- Gamepad -------------------------------------------------------------

    /// Returns true if the specified gamepad is connected.
    virtual bool IsGamepadAvailable(int gamepad) const = 0;

    /// Returns true if the gamepad button is currently held down.
    virtual bool IsGamepadButtonDown(int gamepad, GamepadButton button) const = 0;

    /// Returns true if the gamepad button was pressed this frame.
    virtual bool IsGamepadButtonPressed(int gamepad, GamepadButton button) const = 0;

    /// Axis value in the range [-1.0, 1.0].
    virtual float GetGamepadAxisValue(int gamepad, GamepadAxis axis) const = 0;

    /// Axis value by raw integer code, in the range [-1.0, 1.0].
    virtual float GetGamepadAxisValue(int gamepad, int axis) const = 0;
};

} // namespace RS
