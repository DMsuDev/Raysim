#include "rspch.hpp"

#include "Raysim/Input/EventDrivenInput.hpp"
#include "Raysim/Events/KeyEvent.hpp"
#include "Raysim/Events/MouseEvent.hpp"
#include "Raysim/Events/Events.hpp"

namespace RS {

// ============================================================================
// Lifecycle
// ============================================================================

void EventDrivenInput::Update()
{
    // Clear per-frame transient key states.
    m_KeyPressed.fill(false);
    m_KeyReleased.fill(false);
    m_KeyRepeating.fill(false);
    m_LastKeyPressed = KeyCode::Unknown; // reset: only valid within the frame it occurred

    // Clear per-frame transient mouse states.
    m_MousePressed.fill(false);
    m_MouseReleased.fill(false);

    // Reset mouse delta and scroll (accumulate fresh each frame).
    m_MouseDelta = Math::Vec2f{ 0.0f, 0.0f };
    m_ScrollY    = 0.0f;
}

// ============================================================================
// OnEvent dispatch to typed handlers
// ============================================================================

void EventDrivenInput::OnEvent(Event& e)
{
    EventDispatcher d(e);
    d.Dispatch<KeyPressedEvent>         ([this](KeyPressedEvent&          ke){ HandleKeyPressed(ke);          return false; });
    d.Dispatch<KeyReleasedEvent>        ([this](KeyReleasedEvent&         ke){ HandleKeyReleased(ke);         return false; });
    d.Dispatch<KeyTypedEvent>           ([this](KeyTypedEvent&            ke){ HandleKeyTyped(ke);            return false; });
    d.Dispatch<MouseButtonPressedEvent> ([this](MouseButtonPressedEvent&  me){ HandleMouseButtonPressed(me);  return false; });
    d.Dispatch<MouseButtonReleasedEvent>([this](MouseButtonReleasedEvent& me){ HandleMouseButtonReleased(me); return false; });
    d.Dispatch<MouseMovedEvent>         ([this](MouseMovedEvent&          me){ HandleMouseMoved(me);          return false; });
    d.Dispatch<MouseScrolledEvent>      ([this](MouseScrolledEvent&       me){ HandleMouseScrolled(me);       return false; });
}

// ============================================================================
// Internal event handlers
// ============================================================================

void EventDrivenInput::HandleKeyPressed(const KeyPressedEvent& e)
{
    const auto idx = static_cast<std::size_t>(e.GetKeyCode());
    if (idx == 0 || idx >= KeyCount) return;

    m_KeyDown[idx]      = true;
    m_KeyPressed[idx]   = !e.IsRepeat(); // Only "fresh" presses count as Pressed
    m_KeyRepeating[idx] = e.IsRepeat();

    // Only a fresh press updates the last-key tracker.
    if (!e.IsRepeat())
        m_LastKeyPressed = e.GetKeyCode();

    // Update modifier bitmask
    m_Modifiers = Modifier::None;
    if (m_KeyDown[static_cast<std::size_t>(KeyCode::LeftShift)]   ||
        m_KeyDown[static_cast<std::size_t>(KeyCode::RightShift)])   m_Modifiers |= Modifier::Shift;
    if (m_KeyDown[static_cast<std::size_t>(KeyCode::LeftControl)]  ||
        m_KeyDown[static_cast<std::size_t>(KeyCode::RightControl)]) m_Modifiers |= Modifier::Control;
    if (m_KeyDown[static_cast<std::size_t>(KeyCode::LeftAlt)]      ||
        m_KeyDown[static_cast<std::size_t>(KeyCode::RightAlt)])     m_Modifiers |= Modifier::Alt;
    if (m_KeyDown[static_cast<std::size_t>(KeyCode::LeftSuper)]    ||
        m_KeyDown[static_cast<std::size_t>(KeyCode::RightSuper)])   m_Modifiers |= Modifier::Super;
}

void EventDrivenInput::HandleKeyReleased(const KeyReleasedEvent& e)
{
    const auto idx = static_cast<std::size_t>(e.GetKeyCode());
    if (idx == 0 || idx >= KeyCount) return;

    m_KeyDown[idx]     = false;
    m_KeyReleased[idx] = true;

    // Recompute modifiers (a modifier key was released).
    m_Modifiers = Modifier::None;
    if (m_KeyDown[static_cast<std::size_t>(KeyCode::LeftShift)]   ||
        m_KeyDown[static_cast<std::size_t>(KeyCode::RightShift)])   m_Modifiers |= Modifier::Shift;
    if (m_KeyDown[static_cast<std::size_t>(KeyCode::LeftControl)]  ||
        m_KeyDown[static_cast<std::size_t>(KeyCode::RightControl)]) m_Modifiers |= Modifier::Control;
    if (m_KeyDown[static_cast<std::size_t>(KeyCode::LeftAlt)]      ||
        m_KeyDown[static_cast<std::size_t>(KeyCode::RightAlt)])     m_Modifiers |= Modifier::Alt;
    if (m_KeyDown[static_cast<std::size_t>(KeyCode::LeftSuper)]    ||
        m_KeyDown[static_cast<std::size_t>(KeyCode::RightSuper)])   m_Modifiers |= Modifier::Super;
}

void EventDrivenInput::HandleKeyTyped(const KeyTypedEvent& e)
{
    m_CharQueue.push(e.GetCodepoint());
}

uint32_t EventDrivenInput::GetCharPressed()
{
    if (m_CharQueue.empty())
        return 0;
    const uint32_t cp = m_CharQueue.front();
    m_CharQueue.pop();
    return cp;
}

void EventDrivenInput::HandleMouseButtonPressed(const MouseButtonPressedEvent& e)
{
    const auto idx = static_cast<std::size_t>(e.GetButton());
    if (idx >= ButtonCount) return;
    m_MouseDown[idx]    = true;
    m_MousePressed[idx] = true;
}

void EventDrivenInput::HandleMouseButtonReleased(const MouseButtonReleasedEvent& e)
{
    const auto idx = static_cast<std::size_t>(e.GetButton());
    if (idx >= ButtonCount) return;
    m_MouseDown[idx]     = false;
    m_MouseReleased[idx] = true;
}

void EventDrivenInput::HandleMouseMoved(const MouseMovedEvent& e)
{
    const Math::Vec2f newPos{ e.GetX(), e.GetY() };
    m_MouseDelta    = newPos - m_MousePosition;
    m_MousePosition = newPos;
}

void EventDrivenInput::HandleMouseScrolled(const MouseScrolledEvent& e)
{
    m_ScrollY += e.GetYOffset(); // Accumulate within the frame
}

// ============================================================================
// Keyboard queries
// ============================================================================

bool EventDrivenInput::IsKeyDown(KeyCode key) const
{
    const auto idx = static_cast<std::size_t>(key);
    return idx > 0 && idx < KeyCount && m_KeyDown[idx];
}

bool EventDrivenInput::IsKeyPressed(KeyCode key) const
{
    const auto idx = static_cast<std::size_t>(key);
    return idx > 0 && idx < KeyCount && m_KeyPressed[idx];
}

bool EventDrivenInput::IsKeyReleased(KeyCode key) const
{
    const auto idx = static_cast<std::size_t>(key);
    return idx > 0 && idx < KeyCount && m_KeyReleased[idx];
}

bool EventDrivenInput::IsKeyRepeating(KeyCode key) const
{
    const auto idx = static_cast<std::size_t>(key);
    return idx > 0 && idx < KeyCount && m_KeyRepeating[idx];
}

KeyCode EventDrivenInput::GetLastKeyPressed() const
{
    return m_LastKeyPressed;
}

Modifier EventDrivenInput::GetModifiers() const
{
    return m_Modifiers;
}

// ============================================================================
// Mouse queries
// ============================================================================

Math::Vec2f EventDrivenInput::GetMousePosition() const { return m_MousePosition; }
Math::Vec2f EventDrivenInput::GetMouseDelta()    const { return m_MouseDelta; }
float       EventDrivenInput::GetMouseWheelMove() const { return m_ScrollY; }

bool EventDrivenInput::IsMouseButtonDown(MouseButton button) const
{
    const auto idx = static_cast<std::size_t>(button);
    return idx < ButtonCount && m_MouseDown[idx];
}

bool EventDrivenInput::IsMouseButtonPressed(MouseButton button) const
{
    const auto idx = static_cast<std::size_t>(button);
    return idx < ButtonCount && m_MousePressed[idx];
}

bool EventDrivenInput::IsMouseButtonReleased(MouseButton button) const
{
    const auto idx = static_cast<std::size_t>(button);
    return idx < ButtonCount && m_MouseReleased[idx];
}

} // namespace RS
