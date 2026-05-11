#pragma once
#include "Raysim/Input/Input.hpp"
#include "Raysim/Events/KeyEvent.hpp"

#include <queue>

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

    // Clears per-frame transient state. Must be called once per frame before PollEvents.
    void Update() override;

    // Captures KeyTypedEvent to populate the internal character queue.
    void OnEvent(Event& e) override;

    // ============================================================================
    // KEYBOARD INPUT
    // ============================================================================

    bool IsKeyDown(KeyCode key) const override;
    bool IsKeyPressed(KeyCode key) const override;
    bool IsKeyReleased(KeyCode key) const override;
    bool IsKeyRepeating(KeyCode key) const override;

    KeyCode  GetLastKeyPressed() const override;
    ScanCode GetScanCode(KeyCode key) const override;
    KeyCode  GetKeyFromScanCode(ScanCode sc) const override;
    Modifier GetModifiers() const override;
    uint32_t GetCharPressed() override;

    // ============================================================================
    // MOUSE INPUT
    // ============================================================================

    Math::Vec2f GetMousePosition() const override;
    Math::Vec2f GetMouseDelta() const override;

    float GetMouseWheelMove() const override;

    bool IsMouseButtonDown(MouseButton button) const override;
    bool IsMouseButtonPressed(MouseButton button) const override;
    bool IsMouseButtonReleased(MouseButton button) const override;

    // ============================================================================
    // Cursor Methods
    // ============================================================================

    void SetCursorPosition(float x, float y) override;
    void SetCursorVisible(bool visible) override;

    void SetCursorMode(CursorMode mode) override;
    CursorMode GetCursorMode() const override;

    // ============================================================================
    // GAMEPAD INPUT
    // ============================================================================

    bool IsGamepadAvailable(int gamepad) const override;
    bool IsGamepadButtonDown(int gamepad, GamepadButton button) const override;
    bool IsGamepadButtonPressed(int gamepad, GamepadButton button) const override;
    bool IsGamepadButtonReleased(int gamepad, GamepadButton button) const override;

    float GetGamepadAxisValue(int gamepad, GamepadAxis axis) const override;
    float GetGamepadAxisValue(int gamepad, int axis) const override;

    const char* GetGamepadName(int gamepad) const override;

private:
    KeyCode              m_LastKeyPressed = KeyCode::Unknown;
    CursorMode           m_CursorMode     = CursorMode::Normal;
    std::queue<uint32_t> m_CharQueue;
};

} // namespace RS::Backend
