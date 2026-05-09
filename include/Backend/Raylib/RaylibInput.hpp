#pragma once
#include "Raysim/Input/Input.hpp"

namespace RS::Backend {

/**
 * @class RaylibInput
 * @brief Raylib implementation of the Input interface
 *
 * Provides input handling for keyboard, mouse, and gamepad through raylib's input functions.
 * All input queries are delegated directly to raylib's native functions.
 */
class RaylibInput final : public Input
{
public:
    RaylibInput() = default;
    ~RaylibInput() override = default;

    void Update() override {} // No-op, raylib handles input state internally

    // ============================================================================
    // KEYBOARD INPUT
    // ============================================================================

    bool IsKeyDown(KeyCode key) const override;
    bool IsKeyPressed(KeyCode key) const override;
    bool IsKeyReleased(KeyCode key) const override;
    bool IsKeyRepeating(KeyCode key) const override;
    KeyCode GetLastKeyPressed() const override;

    // ============================================================================
    // MOUSE INPUT
    // ============================================================================

    Math::Vec2f GetMousePosition() const override;
    Math::Vec2f GetMouseDelta() const override;
    bool IsMouseButtonDown(MouseButton button) const override;
    bool IsMouseButtonPressed(MouseButton button) const override;
    bool IsMouseButtonReleased(MouseButton button) const override;
    float GetMouseWheelMove() const override;
    bool IsMouseOnScreen() const override;

    // ============================================================================
    // GAMEPAD INPUT
    // ============================================================================

    bool IsGamepadAvailable(int gamepad) const override;
    bool IsGamepadButtonDown(int gamepad, GamepadButton button) const override;
    bool IsGamepadButtonPressed(int gamepad, GamepadButton button) const override;
    float GetGamepadAxisValue(int gamepad, GamepadAxis axis) const override;
    float GetGamepadAxisValue(int gamepad, int axis) const override;
};

} // namespace RS::Backend
