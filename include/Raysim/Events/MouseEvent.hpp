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
#include "Raysim/Input/MouseCodes.hpp"

#include <sstream>

namespace RS {

// ============================================================================
// MouseMovedEvent
// ============================================================================

/**
 * @class MouseMovedEvent
 * @brief Fired when the cursor position changes within the window.
 *
 * Coordinates are in window-space pixels (top-left origin).
 */
class MouseMovedEvent final : public Event
{
public:
    MouseMovedEvent(float x, float y) : m_X(x), m_Y(y) {}

    [[nodiscard]] float GetX() const noexcept { return m_X; }
    [[nodiscard]] float GetY() const noexcept { return m_Y; }

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "MouseMovedEvent: (" << m_X << ", " << m_Y << ')';
        return ss.str();
    }

    RS_EVENT_CLASS_TYPE(MouseMoved)
    RS_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    float m_X, m_Y;
};

// ============================================================================
// MouseScrolledEvent
// ============================================================================

/**
 * @class MouseScrolledEvent
 * @brief Fired when the scroll wheel (or touchpad) moves.
 *
 * Offsets are in scroll units. Positive Y is scroll-up; positive X is
 * scroll-right (horizontal wheels / touchpads).
 */
class MouseScrolledEvent final : public Event
{
public:
    MouseScrolledEvent(float xOffset, float yOffset)
        : m_XOffset(xOffset), m_YOffset(yOffset) {}

    [[nodiscard]] float GetXOffset() const noexcept { return m_XOffset; }
    [[nodiscard]] float GetYOffset() const noexcept { return m_YOffset; }

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "MouseScrolledEvent: (" << m_XOffset << ", " << m_YOffset << ')';
        return ss.str();
    }

    RS_EVENT_CLASS_TYPE(MouseScrolled)
    RS_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput)

private:
    float m_XOffset, m_YOffset;
};

// ============================================================================
// MouseButtonEvent  (abstract base)
// ============================================================================

/**
 * @class MouseButtonEvent
 * @brief Abstract base for mouse-button events, carrying the button identifier.
 */
class MouseButtonEvent : public Event
{
public:
    [[nodiscard]] MouseButton GetButton() const noexcept { return m_Button; }

    RS_EVENT_CLASS_CATEGORY(EventCategoryMouse | EventCategoryInput | EventCategoryMouseButton)

protected:
    explicit MouseButtonEvent(MouseButton button) : m_Button(button) {}
    MouseButton m_Button;
};

// ============================================================================
// MouseButtonPressedEvent
// ============================================================================

/**
 * @class MouseButtonPressedEvent
 * @brief Fired when a mouse button is pressed down.
 */
class MouseButtonPressedEvent final : public MouseButtonEvent
{
public:
    explicit MouseButtonPressedEvent(MouseButton button) : MouseButtonEvent(button) {}

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "MouseButtonPressedEvent: " << static_cast<int>(m_Button);
        return ss.str();
    }

    RS_EVENT_CLASS_TYPE(MouseButtonPressed)
};

// ============================================================================
// MouseButtonReleasedEvent
// ============================================================================

/**
 * @class MouseButtonReleasedEvent
 * @brief Fired when a mouse button is released.
 */
class MouseButtonReleasedEvent final : public MouseButtonEvent
{
public:
    explicit MouseButtonReleasedEvent(MouseButton button) : MouseButtonEvent(button) {}

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "MouseButtonReleasedEvent: " << static_cast<int>(m_Button);
        return ss.str();
    }

    RS_EVENT_CLASS_TYPE(MouseButtonReleased)
};

} // namespace RS
