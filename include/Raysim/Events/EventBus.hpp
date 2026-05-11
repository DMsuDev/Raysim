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

#include <functional>
#include <unordered_map>
#include <vector>
#include <cstdint>

namespace RS {

// ============================================================================
// EventBus
// ============================================================================

/**
 * @class EventBus
 * @brief Type-safe publish/subscribe event bus.
 *
 * Allows any system to subscribe to a specific event type and be notified
 * whenever that event is published, independently of the layer stack.
 *
 * Unlike the layer-based propagation (which is blocking and ordered),
 * the EventBus is a broadcast channel: ALL active subscribers receive
 * the event, in subscription order, regardless of the Handled flag.
 *
 * Typical use cases:
 *   - Audio system reacting to WindowFocusEvent to pause/resume
 *   - Debug overlay subscribing to KeyPressedEvent for hotkeys
 *   - Achievements system listening for game events
 *
 * The bus is owned by the Application and exposed via EngineContext so
 * scenes and layers can access it without going through the Application.
 *
 * @note Not thread-safe. All calls must be made from the main thread.
 *
 * Usage:
 * @code
 *   // Subscribe
 *   auto id = ctx.Bus->Subscribe<KeyPressedEvent>([](const KeyPressedEvent& e) {
 *       RS_TRACE("Key pressed: {}", static_cast<int>(e.GetKeyCode()));
 *   });
 *
 *   // Unsubscribe when done (e.g. in OnDetach)
 *   ctx.Bus->Unsubscribe(id);
 *
 *   // Publish (done automatically by Application for all window events)
 *   ctx.Bus->Publish(myEvent);
 * @endcode
 */
class EventBus
{
public:
    /// Opaque handle returned by Subscribe(). Store it and pass to Unsubscribe().
    using SubscriptionID = uint64_t;

    EventBus()  = default;
    ~EventBus() = default;

    EventBus(const EventBus&)            = delete;
    EventBus& operator=(const EventBus&) = delete;

    // ======================================================================
    // Subscribe
    // ======================================================================

    /**
     * @brief Register a callback for events of type T.
     *
     * The callback receives a const reference to the concrete event, so no
     * cast is required in the handler.
     *
     * @tparam T        Concrete event type (must inherit from Event).
     * @param  callback Callable with signature `void(const T&)`.
     * @return A SubscriberID that can be passed to Unsubscribe() later.
     */
    template<typename T, typename F>
    [[nodiscard]] SubscriptionID Subscribe(F&& callback)
    {
        static_assert(std::is_base_of_v<Event, T>, "T must derive from Event");

        const SubscriptionID id = m_NextID++;
        // Wrap the typed callback into a type-erased Event& handler.
        auto wrapper = [cb = std::forward<F>(callback)](Event& e)
        {
            cb(static_cast<const T&>(e));
        };

        m_Subscribers[T::GetStaticType()].push_back({id, std::move(wrapper)});
        return id;
    }

    // ======================================================================
    // Unsubscribe
    // ======================================================================

    /**
     * @brief Remove a previously registered subscriber.
     *
     * If the ID is not found the call is a no-op.
     *
     * @param id  The `SubscriptionID` returned by `Subscribe()`.
     */
    void Unsubscribe(SubscriptionID id)
    {
        for (auto& [type, subs] : m_Subscribers)
        {
            auto it = std::find_if(subs.begin(), subs.end(),
                [id](const Subscriber& s){ return s.ID == id; });

            if (it != subs.end())
            {
                subs.erase(it);
                return;
            }
        }
    }

    // ======================================================================
    // Publish
    // ======================================================================

    /**
     * @brief Broadcast @p event to all subscribers of its type.
     *
     * All matching subscribers are called in subscription order.
     * The Handled flag on the event does NOT stop bus propagation
     * (use the layer stack for stoppable propagation).
     *
     * @param event The event to broadcast. May be any Event subclass.
     */
    void Publish(Event& event)
    {
        auto it = m_Subscribers.find(event.GetEventType());
        if (it == m_Subscribers.end())
            return;

        // Iterate over a copy in case a callback unsubscribes during dispatch.
        auto subs = it->second;
        for (auto& sub : subs)
            sub.Callback(event);
    }

    // ======================================================================
    // Convenience: publish a temporary rvalue event
    // ======================================================================

    /**
     * @brief Construct and publish an event in one call.
     *
     * Convenience helper equivalent to:
     *     T e(args...);
     *     Publish(e);
     *
     * This does NOT propagate the event through the engine's layer stack and
     * does NOT respect the Handled flag. It is purely a broadcast to EventBus
     * subscribers.
     *
     * @tparam T    Concrete event type.
     * @tparam Args Constructor argument types for T.
     */
    template<typename T, typename... Args>
    void Emit(Args&&... args)
    {
        static_assert(std::is_base_of_v<Event, T>, "T must derive from Event");
        T event(std::forward<Args>(args)...);
        Publish(event);
    }

private:
    using HandlerFn = std::function<void(Event&)>;

    struct Subscriber
    {
        SubscriptionID ID;
        HandlerFn      Callback;
    };

    std::unordered_map<EventType, std::vector<Subscriber>> m_Subscribers;
    SubscriptionID m_NextID = 1;
};

} // namespace RS
