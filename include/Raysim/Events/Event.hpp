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

#include "Raysim/Core/Base.hpp"

#include <functional>
#include <string>

namespace RS {

// ============================================================================
// EventType
// ============================================================================

/**
 * @enum EventType
 * @brief Identifies each distinct event kind dispatched by the engine.
 *
 * Events are blocking: when fired they are dispatched immediately and must
 * be handled before execution continues.
 */
enum class EventType
{
    None = 0,
    WindowClose, WindowResize, WindowFocus,
    KeyPressed, KeyReleased, KeyTyped,
    MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled,
};

// ============================================================================
// EventCategory  (bitmask flags)
// ============================================================================

/**
 * @enum EventCategory
 * @brief Bitmask categories that group related event types.
 *
 * A single event can belong to multiple categories (e.g. KeyPressed is both
 * EventCategoryInput and EventCategoryKeyboard). Use IsInCategory() to test.
 */
enum EventCategory
{
    None                     = 0,
    EventCategoryApplication = BIT(0),
    EventCategoryInput       = BIT(1),
    EventCategoryKeyboard    = BIT(2),
    EventCategoryMouse       = BIT(3),
    EventCategoryMouseButton = BIT(4),
};

// ============================================================================
// Convenience macros  (identical to Hazel's pattern)
// ============================================================================

/// Implements GetEventType(), GetStaticType(), and GetName() for a concrete event class.
#define RS_EVENT_CLASS_TYPE(type)                                                    \
    static  EventType GetStaticType()                { return EventType::type; }     \
    virtual EventType GetEventType()  const override { return GetStaticType(); }     \
    virtual const char* GetName()     const override { return #type; }

/// Implements GetCategoryFlags() for a concrete event class.
#define RS_EVENT_CLASS_CATEGORY(category) \
    virtual int GetCategoryFlags() const override { return category; }

// ============================================================================
// Event  (abstract base)
// ============================================================================

/**
 * @class Event
 * @brief Abstract base class for all engine events.
 *
 * Subclass Event (or use the provided concrete classes) to represent discrete
 * things that happen in the application: key presses, mouse movement, window
 * resize, etc.
 *
 * The Handled flag lets layers stop propagation: once a layer sets Handled to
 * true, subsequent layers will not receive the event.
 */
class Event
{
public:
    virtual ~Event() = default;

    /// Set to true by the first handler that fully processes this event.
    /// Subsequent layers will not receive the event once this is true.
    bool Handled = false;

    [[nodiscard]] virtual EventType    GetEventType()     const = 0;
    [[nodiscard]] virtual const char*  GetName()          const = 0;
    [[nodiscard]] virtual int          GetCategoryFlags() const = 0;
    [[nodiscard]] virtual std::string  ToString()         const { return GetName(); }

    /// Returns true if this event belongs to the specified category.
    [[nodiscard]] bool IsInCategory(EventCategory category) const
    {
        return (GetCategoryFlags() & category) != 0;
    }
};

// ============================================================================
// EventDispatcher
// ============================================================================

/**
 * @class EventDispatcher
 * @brief Type-safe dispatcher that routes an Event to the correct handler.
 *
 * Usage:
 * @code
 *   void MyLayer::OnEvent(Event& e)
 *   {
 *       EventDispatcher d(e);
 *       d.Dispatch<KeyPressedEvent>([this](KeyPressedEvent& ke) {
 *           // handle key press
 *           return true; // mark handled
 *       });
 *   }
 * @endcode
 */
class EventDispatcher
{
public:
    explicit EventDispatcher(Event& event) : m_Event(event) {}

    /// Calls @p func if the wrapped event's type matches T.
    /// Returns true if the type matched (regardless of whether Handled was set).
    /// @tparam T   Concrete event type to match.
    /// @tparam F   Callable with signature `bool(T&)`.
    template<typename T, typename F>
    bool Dispatch(const F& func)
    {
        if (m_Event.GetEventType() == T::GetStaticType())
        {
            m_Event.Handled |= func(static_cast<T&>(m_Event));
            return true;
        }
        return false;
    }

private:
    Event& m_Event;
};

// ============================================================================
// Stream operator (useful with RS_CORE_TRACE / spdlog)
// ============================================================================

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
    return os << e.ToString();
}

} // namespace RS
