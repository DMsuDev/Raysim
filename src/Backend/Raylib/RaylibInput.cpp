#include "../../pch.hpp"

#include "Raysim/Backend/Raylib/RaylibInput.hpp"

#include <raylib.h>

namespace RS {

/** @brief Helper function to convert enum values to raylib's expected integer codes
 *
 * @tparam T The enum type (e.g., KeyCode, MouseButton)
 * @param value The enum value to convert
 * @return The corresponding integer code for raylib functions
 */
template<typename T>
static int ToRayLibKey(T value) {
    return static_cast<int>(value);
}

// ============================================================================
// KEYBOARD INPUT
// ============================================================================

bool RaylibInput::IsKeyDown(KeyCode key) const {
    return ::IsKeyDown(ToRayLibKey(key));
}

bool RaylibInput::IsKeyPressed(KeyCode key) const {
    return ::IsKeyPressed(ToRayLibKey(key));
}

bool RaylibInput::IsKeyReleased(KeyCode key) const {
    return ::IsKeyReleased(ToRayLibKey(key));
}

bool RaylibInput::IsKeyRepeating(KeyCode key) const {
    return ::IsKeyPressedRepeat(ToRayLibKey(key));
}

KeyCode RaylibInput::GetLastKeyPressed() const {
    return static_cast<KeyCode>(::GetKeyPressed());
}

// ============================================================================
// MOUSE INPUT
// ============================================================================

Vector2 RaylibInput::GetMousePosition() const {
    auto pos = ::GetMousePosition();
    return {pos.x, pos.y};
}

Vector2 RaylibInput::GetMouseDelta() const {
    auto delta = ::GetMouseDelta();
    return {delta.x, delta.y};
}

bool RaylibInput::IsMouseButtonDown(MouseButton button) const {
    return ::IsMouseButtonDown(ToRayLibKey(button));
}

bool RaylibInput::IsMouseButtonPressed(MouseButton button) const {
    return ::IsMouseButtonPressed(ToRayLibKey(button));
}

bool RaylibInput::IsMouseButtonReleased(MouseButton button) const {
    return ::IsMouseButtonReleased(ToRayLibKey(button));
}

float RaylibInput::GetMouseWheelMove() const {
    return ::GetMouseWheelMove();
}

bool RaylibInput::IsMouseOnScreen() const {
    return ::IsCursorOnScreen();
}

// ============================================================================
// GAMEPAD INPUT
// ============================================================================

bool RaylibInput::IsGamepadAvailable(int gamepad) const {
    return ::IsGamepadAvailable(gamepad);
}

bool RaylibInput::IsGamepadButtonDown(int gamepad, GamepadButton button) const {
    return ::IsGamepadButtonDown(gamepad, ToRayLibKey(button));
}

bool RaylibInput::IsGamepadButtonPressed(int gamepad, GamepadButton button) const {
    return ::IsGamepadButtonPressed(gamepad, ToRayLibKey(button));
}

float RaylibInput::GetGamepadAxisValue(int gamepad, GamepadAxis axis) const {
    return ::GetGamepadAxisMovement(gamepad, ToRayLibKey(axis));
}

float RaylibInput::GetGamepadAxisValue(int gamepad, int axis) const {
    return ::GetGamepadAxisMovement(gamepad, axis);
}

} // namespace RS
