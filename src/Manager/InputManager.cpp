#include "Raysim/Input/InputManager.hpp"

#include <raylib.h>

namespace RS {

// ============================================================================
// KEYBOARD INPUT
// ============================================================================

bool InputManager::IsKeyDown(KeyCode key) const {
    return ::IsKeyDown(static_cast<int>(key));
}

bool InputManager::IsKeyPressed(KeyCode key) const {
    return ::IsKeyPressed(static_cast<int>(key));
}

bool InputManager::IsKeyReleased(KeyCode key) const {
    return ::IsKeyReleased(static_cast<int>(key));
}

bool InputManager::IsKeyRepeating(KeyCode key) const {
    return ::IsKeyDown(static_cast<int>(key));
}

KeyCode InputManager::GetLastKeyPressed() const {
    return static_cast<KeyCode>(::GetKeyPressed());
}

// ============================================================================
// MOUSE INPUT
// ============================================================================

Vector2 InputManager::GetMousePosition() const {
    Vector2 pos = {::GetMousePosition().x, ::GetMousePosition().y};
    return {pos.x, pos.y};
}

Vector2 InputManager::GetMouseDelta() const {
    Vector2 delta = {::GetMouseDelta().x, ::GetMouseDelta().y};
    return {delta.x, delta.y};
}

bool InputManager::IsMouseButtonDown(MouseButton button) const {
    return ::IsMouseButtonDown(static_cast<int>(button));
}

bool InputManager::IsMouseButtonPressed(MouseButton button) const {
    return ::IsMouseButtonPressed(static_cast<int>(button));
}

bool InputManager::IsMouseButtonReleased(MouseButton button) const {
    return ::IsMouseButtonReleased(static_cast<int>(button));
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
    return ::IsGamepadButtonDown(gamepad, static_cast<int>(button));
}

bool InputManager::IsGamepadButtonPressed(int gamepad, GamepadButton button) const {
    return ::IsGamepadButtonPressed(gamepad, static_cast<int>(button));
}

float InputManager::GetGamepadAxisValue(int gamepad, GamepadAxis axis) const {
    return ::GetGamepadAxisMovement(gamepad, static_cast<int>(axis));
}

// ============================================================================
// UTILITY
// ============================================================================

void InputManager::Update() {
    Vector2 pos = {::GetMousePosition().x, ::GetMousePosition().y};
    lastMousePos_ = pos;
}

void InputManager::Reset() {
    lastMousePos_ = {0, 0};
}

} // namespace RS
