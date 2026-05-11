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
 * @brief Base class for all keyboard-related events.
 *
 * Represents a keyboard action and carries both the abstract layout‑independent
 * KeyCode and the backend‑specific physical ScanCode.
 *
 * - KeyCode: logical key identifier used by the engine (layout‑agnostic).
 *
 * - ScanCode: physical key identifier provided by the backend. This allows
 *   distinguishing physical keys even when keyboard layout changes.
 *
 * If the backend does not provide a physical scan code, m_ScanCode is set to
 * InvalidScanCode.
 */
class KeyEvent : public Event
{
public:
    [[nodiscard]] KeyCode  GetKeyCode()  const noexcept { return m_KeyCode;  }

    /// Physical key identifier supplied by the backend, or InvalidScanCode if unavailable.
    [[nodiscard]] ScanCode GetScanCode() const noexcept { return m_ScanCode; }

    RS_EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)

protected:
    KeyEvent(KeyCode keycode, ScanCode scancode = InvalidScanCode)
        : m_KeyCode(keycode), m_ScanCode(scancode) {}

    KeyCode  m_KeyCode;
    ScanCode m_ScanCode;
};

// ============================================================================
// KeyPressedEvent
// ============================================================================

/**
 * @class KeyPressedEvent
 * @brief Fired when a key transitions from "not pressed" to "pressed".
 *
 * This event is also generated repeatedly while the key is held down if the
 * operating system's auto‑repeat feature is active.
 *
 * The `isRepeat` flag indicates whether the event is an initial press or an auto‑repeat.
 */
class KeyPressedEvent final : public KeyEvent
{
public:
    /// @param keycode  Abstract key code (layout-independent).
    /// @param isRepeat True when the OS auto-repeat mechanism re-fires the event.
    /// @param scancode Physical key identifier from the backend (optional).
    KeyPressedEvent(KeyCode keycode, bool isRepeat = false, ScanCode scancode = InvalidScanCode)
        : KeyEvent(keycode, scancode), m_IsRepeat(isRepeat) {}

    [[nodiscard]] bool IsRepeat() const noexcept { return m_IsRepeat; }

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "KeyPressedEvent: " << static_cast<int>(m_KeyCode)
           << " sc=" << m_ScanCode
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
 * @brief Fired when a key transitions from "pressed" to "not pressed".
 *
 * This event is emitted exactly once per key release and never repeats.
 * It carries both the abstract KeyCode and the backend physical ScanCode.
 */
class KeyReleasedEvent final : public KeyEvent
{
public:
    /// @param keycode  Abstract key code (layout-independent).
    /// @param scancode Physical key identifier from the backend (optional).
    KeyReleasedEvent(KeyCode keycode, ScanCode scancode = InvalidScanCode)
        : KeyEvent(keycode, scancode) {}

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "KeyReleasedEvent: " << static_cast<int>(m_KeyCode)
           << " sc=" << m_ScanCode;
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
