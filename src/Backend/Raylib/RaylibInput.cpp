#include "rspch.hpp"

#include "Backend/Raylib/RaylibInput.hpp"
#include "Backend/Raylib/RaylibKeyMap.hpp"

#include "Raysim/Events/KeyEvent.hpp"
#include "Raysim/Events/Events.hpp"

#include <raylib.h>

namespace RS::Backend {

// ============================================================================
// LIFECYCLE
// ============================================================================

void RaylibInput::Update()
{
    RS_PROFILE_FUNCTION();

    // Nothing to clear: Raylib manages key state internally.
    // The char queue is drained by GetCharPressed(); do NOT clear it here
    // because Update() is called before PollEvents(), and new chars arrive
    // via OnEvent() after PollEvents(). Clearing here would discard them.
    //
    // Reset last-pressed-key: only valid within the frame of the press.
    // PollEvents() will repopulate it via OnEvent(KeyPressedEvent) if needed.
    m_LastKeyPressed = KeyCode::Unknown;
}

void RaylibInput::OnEvent(Event& e)
{
    RS_PROFILE_FUNCTION();
    EventDispatcher d(e);

    // Collect typed characters dispatched by RaylibWindow::PollEvents().
    d.Dispatch<KeyTypedEvent>([this](KeyTypedEvent& ke)
    {
        m_CharQueue.push(ke.GetCodepoint());
        return false;
    });

    // Track last pressed key (non-repeat only) so GetLastKeyPressed() is idempotent
    // within a frame. We cannot use ::GetKeyPressed() for this because it is a
    // destructive queue-drain: calling it twice gives different results.
    d.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& ke)
    {
        if (!ke.IsRepeat())
            m_LastKeyPressed = ke.GetKeyCode();
        return false;
    });
}

// ============================================================================
// KEYBOARD INPUT
// ============================================================================

static inline int ToRaylibInt(KeyCode key) noexcept
{
    const auto idx = static_cast<std::size_t>(key);
    return idx < KeyCodeToRaylib.size() ? KeyCodeToRaylib[idx] : 0;
}

bool RaylibInput::IsKeyDown(KeyCode key) const
{
    return ::IsKeyDown(ToRaylibInt(key));
}

bool RaylibInput::IsKeyPressed(KeyCode key) const
{
    return ::IsKeyPressed(ToRaylibInt(key));
}

bool RaylibInput::IsKeyReleased(KeyCode key) const
{
    return ::IsKeyReleased(ToRaylibInt(key));
}

bool RaylibInput::IsKeyRepeating(KeyCode key) const
{
    return ::IsKeyPressedRepeat(ToRaylibInt(key));
}

KeyCode RaylibInput::GetLastKeyPressed() const
{
    // Returns the key cached by OnEvent(KeyPressedEvent), which is idempotent
    // within a frame. Do NOT call ::GetKeyPressed() here: it is a destructive
    // queue-drain and would return different values on successive calls.
    return m_LastKeyPressed;
}

ScanCode RaylibInput::GetScanCode(KeyCode key) const {
    const auto idx = static_cast<std::size_t>(key);
    if (key == KeyCode::Unknown || idx >= KeyCodeToRaylib.size())
        return InvalidScanCode;
    const int raylibKey = KeyCodeToRaylib[idx];
    return raylibKey != 0 ? static_cast<ScanCode>(raylibKey) : InvalidScanCode;
}

KeyCode RaylibInput::GetKeyFromScanCode(ScanCode sc) const
{
    return sc > 0 ? RaylibKeyToKeyCode(static_cast<int>(sc)) : KeyCode::Unknown;
}

Modifier RaylibInput::GetModifiers() const {
    Modifier mods = Modifier::None;
    if (::IsKeyDown(KEY_LEFT_SHIFT)   || ::IsKeyDown(KEY_RIGHT_SHIFT))   mods |= Modifier::Shift;
    if (::IsKeyDown(KEY_LEFT_CONTROL) || ::IsKeyDown(KEY_RIGHT_CONTROL)) mods |= Modifier::Control;
    if (::IsKeyDown(KEY_LEFT_ALT)     || ::IsKeyDown(KEY_RIGHT_ALT))     mods |= Modifier::Alt;
    if (::IsKeyDown(KEY_LEFT_SUPER)   || ::IsKeyDown(KEY_RIGHT_SUPER))   mods |= Modifier::Super;
    return mods;
}

uint32_t RaylibInput::GetCharPressed()
{
    if (m_CharQueue.empty())
        return 0;
    const uint32_t cp = m_CharQueue.front();
    m_CharQueue.pop();
    return cp;
}

// ============================================================================
// MOUSE INPUT
// ============================================================================

Math::Vec2f RaylibInput::GetMousePosition() const
{
    ::Vector2 pos = ::GetMousePosition();
    return Math::Vec2f{pos.x, pos.y};
}

Math::Vec2f RaylibInput::GetMouseDelta() const
{
    ::Vector2 delta = ::GetMouseDelta();
    return Math::Vec2f{delta.x, delta.y};
}

bool RaylibInput::IsMouseButtonDown(MouseButton button) const
{
    const int native = ToRaylibMouseButton(button);
    return native >= 0 && ::IsMouseButtonDown(native);
}

bool RaylibInput::IsMouseButtonPressed(MouseButton button) const
{
    const int native = ToRaylibMouseButton(button);
    return native >= 0 && ::IsMouseButtonPressed(native);
}

bool RaylibInput::IsMouseButtonReleased(MouseButton button) const
{
    const int native = ToRaylibMouseButton(button);
    return native >= 0 && ::IsMouseButtonReleased(native);
}

float RaylibInput::GetMouseWheelMove() const
{
    return ::GetMouseWheelMove();
}

void RaylibInput::SetCursorPosition(float x, float y)
{
    ::SetMousePosition(static_cast<int>(x), static_cast<int>(y));
}

void RaylibInput::SetCursorVisible(bool visible)
{
    // Delegate to SetCursorMode so we never accidentally override a Locked state.
    SetCursorMode(visible ? CursorMode::Normal : CursorMode::Hidden);
}

void RaylibInput::SetCursorMode(CursorMode mode)
{
    switch (mode)
    {
        case CursorMode::Normal:  ::EnableCursor();  break;
        case CursorMode::Hidden:  ::HideCursor();    break;
        case CursorMode::Locked:  ::DisableCursor(); break;
    }
    m_CursorMode = mode;
}

CursorMode RaylibInput::GetCursorMode() const
{
    return m_CursorMode;
}

// ============================================================================
// GAMEPAD INPUT
// ============================================================================

bool RaylibInput::IsGamepadAvailable(int gamepad) const
{
    return ::IsGamepadAvailable(gamepad);
}

bool RaylibInput::IsGamepadButtonDown(int gamepad, GamepadButton button) const
{
    const int native = ToRaylibGamepadButton(button);
    return native >= 0 && ::IsGamepadButtonDown(gamepad, native);
}

bool RaylibInput::IsGamepadButtonPressed(int gamepad, GamepadButton button) const
{
    const int native = ToRaylibGamepadButton(button);
    return native >= 0 && ::IsGamepadButtonPressed(gamepad, native);
}

bool RaylibInput::IsGamepadButtonReleased(int gamepad, GamepadButton button) const
{
    const int native = ToRaylibGamepadButton(button);
    return native >= 0 && ::IsGamepadButtonReleased(gamepad, native);
}

float RaylibInput::GetGamepadAxisValue(int gamepad, GamepadAxis axis) const
{
    return ::GetGamepadAxisMovement(gamepad, ToRaylibGamepadAxis(axis));
}

float RaylibInput::GetGamepadAxisValue(int gamepad, int axis) const
{
    return ::GetGamepadAxisMovement(gamepad, axis);
}

const char* RaylibInput::GetGamepadName(int gamepad) const
{
    return ::GetGamepadName(gamepad);
}

} // namespace RS::Backend
