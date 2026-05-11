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

#include <sstream>

namespace RS {

// ============================================================================
// WindowResizeEvent
// ============================================================================

/**
 * @class WindowResizeEvent
 * @brief Fired when the window client area changes size.
 *
 * Width and height reflect the new client area in screen coordinates.
 * Both may be zero when the window is minimized.
 */
class WindowResizeEvent final : public Event
{
public:
    WindowResizeEvent(uint32_t width, uint32_t height)
        : m_Width(width), m_Height(height) {}

    [[nodiscard]] uint32_t GetWidth()  const noexcept { return m_Width; }
    [[nodiscard]] uint32_t GetHeight() const noexcept { return m_Height; }

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "WindowResizeEvent: " << m_Width << 'x' << m_Height;
        return ss.str();
    }

    RS_EVENT_CLASS_TYPE(WindowResize)
    RS_EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
    uint32_t m_Width, m_Height;
};

// ============================================================================
// WindowCloseEvent
// ============================================================================

/**
 * @class WindowCloseEvent
 * @brief Fired when the user requests to close the window (e.g. clicks the X button).
 *
 * The Application handles this event by setting m_Running = false.
 * Layers can intercept it to show a "are you sure?" dialog and mark it Handled
 * to cancel the shutdown.
 */
class WindowCloseEvent final : public Event
{
public:
    WindowCloseEvent() = default;

    RS_EVENT_CLASS_TYPE(WindowClose)
    RS_EVENT_CLASS_CATEGORY(EventCategoryApplication)
};

// ============================================================================
// WindowFocusEvent
// ============================================================================

/**
 * @class WindowFocusEvent
 * @brief Fired when the window gains or loses OS focus.
 *
 * @p gained is true when focus is gained, false when it is lost.
 */
class WindowFocusEvent final : public Event
{
public:
    explicit WindowFocusEvent(bool gained) : m_Gained(gained) {}

    /// Returns true if the window just gained focus.
    [[nodiscard]] bool IsGained() const noexcept { return m_Gained; }

    [[nodiscard]] std::string ToString() const override
    {
        std::ostringstream ss;
        ss << "WindowFocusEvent: " << (m_Gained ? "gained" : "lost");
        return ss.str();
    }

    RS_EVENT_CLASS_TYPE(WindowFocus)
    RS_EVENT_CLASS_CATEGORY(EventCategoryApplication)

private:
    bool m_Gained;
};

} // namespace RS
