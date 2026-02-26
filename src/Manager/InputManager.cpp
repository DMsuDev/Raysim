#include "Raysim/Input/InputManager.hpp"

#include <raylib.h>

namespace RS {

/** @brief Helper function to convert enum class values to raylib's expected integer codes
 *
 * This template function allows us to use strongly-typed enums (like KeyCode and MouseButton)
 * while still calling raylib functions that expect plain integers. It simply casts the enum
 * value to an int, which is safe as long as the enum values correspond to raylib's key/button codes.
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

bool InputManager::IsKeyDown(KeyCode key) const {
    return ::IsKeyDown(ToRayLibKey(key));
}

bool InputManager::IsKeyPressed(KeyCode key) const {
    return ::IsKeyPressed(ToRayLibKey(key));
}

bool InputManager::IsKeyReleased(KeyCode key) const {
    return ::IsKeyReleased(ToRayLibKey(key));
}

bool InputManager::IsKeyRepeating(KeyCode key) const {
    return ::IsKeyDown(ToRayLibKey(key));
}

KeyCode InputManager::GetLastKeyPressed() const {
    return static_cast<KeyCode>(::GetKeyPressed());
}

// ============================================================================
// MOUSE INPUT
// ============================================================================

Vector2 InputManager::GetMousePosition() const {
    auto pos = ::GetMousePosition();
    return {pos.x, pos.y};
}

Vector2 InputManager::GetMouseDelta() const {
    auto delta = ::GetMouseDelta();
    return {delta.x, delta.y};
}

bool InputManager::IsMouseButtonDown(MouseButton button) const {
    return ::IsMouseButtonDown(ToRayLibKey(button));
}

bool InputManager::IsMouseButtonPressed(MouseButton button) const {
    return ::IsMouseButtonPressed(ToRayLibKey(button));
}

bool InputManager::IsMouseButtonReleased(MouseButton button) const {
    return ::IsMouseButtonReleased(ToRayLibKey(button));
}

float InputManager::GetMouseWheelMove() const {
    return ::GetMouseWheelMove();
}

bool InputManager::IsMouseOnScreen() const {
    return ::IsCursorOnScreen();
}

// ============================================================================
// GAMEPAD INPUT
// ============================================================================

bool InputManager::IsGamepadAvailable(int gamepad) const {
    return ::IsGamepadAvailable(gamepad);
}

bool InputManager::IsGamepadButtonDown(int gamepad, GamepadButton button) const {
    return ::IsGamepadButtonDown(gamepad, ToRayLibKey(button));
}

bool InputManager::IsGamepadButtonPressed(int gamepad, GamepadButton button) const {
    return ::IsGamepadButtonPressed(gamepad, ToRayLibKey(button));
}

float InputManager::GetGamepadAxisValue(int gamepad, GamepadAxis axis) const {
    return ::GetGamepadAxisMovement(gamepad, ToRayLibKey(axis));
}

} // namespace RS
