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
#include "Raysim/Core/WindowProperties.hpp"

#include "Raysim/Math/Types/Vector2.hpp"

#include <functional>

namespace RS {

class Event; // Forward declaration of Event class

/**
 * @interface Window
 * @brief Abstract interface for a window in the application.
 *
 * Defines the essential operations that any window implementation
 * must provide, such as updating, resizing, toggling fullscreen mode.
 */
class Window
{
public:
    virtual ~Window() = default;

// ===========================================================
// Event callback
// ===========================================================

    /// @brief Callable type for the window-level event callback.
    ///
    /// The Application sets this once during construction. The Window
    /// implementation calls it for every event it synthesizes (key press,
    /// resize, close request, etc.).
    using EventCallbackFn = std::function<void(Event&)>;

    /// @brief Register the function that receives all window-level events.
    /// @param callback  A callable with signature `void(Event&)`.
    void SetEventCallback(EventCallbackFn callback)
    {
        RS_CORE_ASSERT(callback, "Event callback cannot be null");
        m_EventCallback = std::move(callback);
        RS_CORE_DEBUG("Window event callback registered");
    }

// ===========================================================
// Non-virtual interface (NVI)
// ===========================================================

    void PollEvents()
    {
        RS_PROFILE_FUNCTION();
        ImplPollEvents();
    }
    void SwapBuffers()
    {
        RS_PROFILE_FUNCTION();
        ImplSwapBuffers();
    }

    [[nodiscard]] bool ShouldClose() const noexcept { return ImplShouldClose(); }

    void SetSize(int width, int height)
    {
        RS_CORE_ASSERT(width > 0 && height > 0, "Window size must be positive.");
        RS_PROFILE_FUNCTION();

        RS_CORE_DEBUG("Setting window size to {}x{}", width, height);

        ImplSetSize(width, height);
    }
    void SetSize(const Math::Vec2i& size) { SetSize(size.x, size.y); }

    Math::Vec2i GetSize() const { return ImplGetSize(); }
    int GetWidth()       const { return ImplGetSize().x; }
    int GetHeight()      const { return ImplGetSize().y; }

    void GetFramebufferSize(int& width, int& height) const
    {
        ImplGetFramebufferSize(width, height);
    }

    void SetFullscreen(bool fullscreen)
    {
        RS_PROFILE_FUNCTION();
        ImplSetFullscreen(fullscreen);
    }
    [[nodiscard]] bool IsFullscreen() const noexcept
    {
        return ImplIsFullscreen();
    }

    void SetVSync(bool enabled)
    {
        ImplSetVSync(enabled);
    }
    [[nodiscard]] bool IsVSync() const noexcept
    {
        return ImplIsVSync();
    }

    [[nodiscard]] bool IsMinimized() const noexcept
    {
        return ImplIsMinimized();
    }

    void SetTitle(const std::string& title)
    {
        RS_CORE_ASSERT(!title.empty(), "Window title cannot be empty.");
        RS_CORE_DEBUG("Setting window title to '{}'", title);

        m_Title = title;
        ImplSetTitle(title);
    }

    [[nodiscard]] const std::string& GetTitle() const noexcept { return m_Title; }

    void* GetNativeWindow() const noexcept
    {
        return ImplGetNativeWindow();
    }

    template<typename T>
    T* GetNativeWindow() const noexcept
    {
        return static_cast<T*>(ImplGetNativeWindow());
    }

    [[nodiscard]] float GetAspectRatio() const noexcept
    {
        auto size = ImplGetSize();
        return size.y != 0 ? static_cast<float>(size.x) / static_cast<float>(size.y) : 0.0f;
    }

protected:

// ===========================================================
// Constructor
// ===========================================================

   explicit Window(const WindowProps& props)
        : m_Title(props.Title)
    {
        // The actual window creation logic will be implemented in the derived class.
    }

    /// Callback set by the Application. Call this from ImplPollEvents() for each
    /// synthesized event. Guard with `if (m_EventCallback)` before calling.
    EventCallbackFn m_EventCallback;

// ===========================================================
// Window Lifecycle
// ===========================================================

    /// @brief Updates the event loop and processes any pending events (e.g., input, window messages).
    virtual void ImplPollEvents() = 0;

    /// @brief Swaps the front and back buffers, presenting the rendered frame to the screen.
    virtual void ImplSwapBuffers() = 0;

    /// @brief Checks if the window should close (e.g., if the user has requested to close it).
    virtual bool ImplShouldClose() const = 0;

// ===========================================================
// Window size properties
// ===========================================================

    /// @brief Sets the size of the window.
    /// @param width The new width of the window.
    /// @param height The new height of the window.
    virtual void ImplSetSize(int width, int height) = 0;

    /// @brief Retrieves the size of the window as a Vec2Int (width, height).
    virtual Math::Vec2i ImplGetSize()   const = 0;

// ===========================================================
// Window mode properties
// ===========================================================

    /// @brief Sets the window to fullscreen mode.
    /// @param fullscreen True to enable fullscreen, false to disable.
    virtual void ImplSetFullscreen(bool fullscreen) = 0;

    virtual bool ImplIsFullscreen() const = 0;

    virtual bool ImplIsMinimized() const = 0;

// ===========================================================
// Frame Buffering
// ===========================================================

    virtual void ImplGetFramebufferSize(int& width, int& height) const = 0;

// ===========================================================
// VSync
// ===========================================================

    /// @brief Enables or disables vertical synchronization (VSync).
    /// @param enabled True to enable VSync, false to disable.
    virtual void ImplSetVSync(bool enabled) = 0;

    /// @brief Checks if vertical synchronization (VSync) is enabled.
    virtual bool ImplIsVSync() const = 0;

// ===========================================================
// Window title
// ===========================================================

    /// @brief Sets the title of the window.
    virtual void ImplSetTitle(const std::string& title) = 0;

// ===========================================================
// Native handle
// ===========================================================

    /// @brief Retrieves the underlying native window handle (e.g GLFWwindow*, HWND).
    /// @return A platform-specific pointer (e.g., GLFWwindow*, HWND, etc.).
    virtual void* ImplGetNativeWindow() const = 0;

private:
    std::string m_Title;
};

} // namespace RS
