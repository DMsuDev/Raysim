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

#include "Raysim/Events/Event.hpp"
#include "Raysim/Input/KeyCodes.hpp"

#include <sstream>

namespace RS {

// ============================================================================
// KeyEvent  (abstract base for all keyboard events)
// ============================================================================

/**
 * @class KeyEvent
 * @brief Abstract base for keyboard events, carrying the key code.
 */
class KeyEvent : public Event
{
public:
    [[nodiscard]] KeyCode GetKeyCode() const noexcept { return m_KeyCode; }

    RS_EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
    explicit KeyEvent(KeyCode keycode) : m_KeyCode(keycode) {}
    KeyCode m_KeyCode;
};

// ============================================================================
// KeyPressedEvent
// ============================================================================

/**
 * @class KeyPressedEvent
 * @brief Fired when a key transitions from not-pressed to pressed.
 *
 * @p isRepeat is true when the OS auto-repeat mechanism re-fires the event
 * while the key is held down.
 */
class KeyPressedEvent final : public KeyEvent
{
public:
    explicit KeyPressedEvent(KeyCode keycode, bool isRepeat = false)
        : KeyEvent(keycode), m_IsRepeat(isRepeat) {}

    [[nodiscard]] bool IsRepeat() const noexcept { return m_IsRepeat; }

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "KeyPressedEvent: " << static_cast<int>(m_KeyCode)
           << " (repeat=" << m_IsRepeat << ')';
        return ss.str();
    }

    RS_EVENT_CLASS_TYPE(KeyPressed)

private:
    bool m_IsRepeat;
};

// ============================================================================
// KeyReleasedEvent
// ============================================================================

/**
 * @class KeyReleasedEvent
 * @brief Fired when a key transitions from pressed to not-pressed.
 */
class KeyReleasedEvent final : public KeyEvent
{
public:
    explicit KeyReleasedEvent(KeyCode keycode) : KeyEvent(keycode) {}

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "KeyReleasedEvent: " << static_cast<int>(m_KeyCode);
        return ss.str();
    }

    RS_EVENT_CLASS_TYPE(KeyReleased)
};

// ============================================================================
// KeyTypedEvent
// ============================================================================

/**
 * @class KeyTypedEvent
 * @brief Fired for each Unicode character produced by the keyboard.
 *
 * Unlike KeyPressedEvent (which carries a layout-independent KeyCode), this
 * event carries the actual Unicode codepoint so text input boxes and chat
 * fields can react to the character the user typed, not the physical key.
 *
 * @note Dead-key combinations and IME composition are backend-dependent.
 */
class KeyTypedEvent final : public Event
{
public:
    explicit KeyTypedEvent(uint32_t codepoint) : m_Codepoint(codepoint) {}

    /// Unicode codepoint of the character that was typed.
    [[nodiscard]] uint32_t GetCodepoint() const noexcept { return m_Codepoint; }

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "KeyTypedEvent: U+" << std::hex << m_Codepoint;
        return ss.str();
    }

    RS_EVENT_CLASS_TYPE(KeyTyped)
    RS_EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

private:
    uint32_t m_Codepoint;
};

} // namespace RS
