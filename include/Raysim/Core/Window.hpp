#pragma once

#include <string>
#include <functional>

#include "Raysim/Core/Memory.hpp"
#include "Raysim/Math/Vector2.hpp"

namespace RS {

/// @struct WindowProps
/// @brief Configuration properties for creating a window.
struct WindowProps {
    std::string Title  = "Raysim App";  ///< The title of the window.
    uint32_t    Width  = 1600;          ///< The width of the window in pixels.
    uint32_t    Height = 900;           ///< The height of the window in pixels.

    WindowProps() = default;
    WindowProps(const std::string& title, uint32_t width, uint32_t height)
        : Title(title), Width(width), Height(height) {}
};

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

    /// @brief Updates the window (swaps buffers and other per-frame operations).
    virtual void OnUpdate() = 0;

    /// @brief Checks if the window should close (e.g., if the user has requested to close it).
    virtual bool ShouldClose() const = 0;

// ===========================================================
// Window size properties
// ===========================================================

    /// @brief Sets the size of the window.
    /// @param width The new width of the window.
    /// @param height The new height of the window.
    virtual void SetSize(uint32_t width, uint32_t height) = 0;

    /// @brief Sets the size of the window using a Vector2.
    /// @param size A Vector2 containing the new width (x) and height (y) of the window.
    virtual void SetSize(const Vector2& size)         = 0;

    /// @brief Retrieves the size of the window as a Vector2 (width, height).
    virtual Vector2  GetSize()     const = 0;
    
    /// @brief Retrieves the width of the window.
    virtual uint32_t GetWidth()    const = 0;

    /// @brief Retrieves the height of the window.
    virtual uint32_t GetHeight()   const = 0;

    /// @brief Calculates the aspect ratio of the window.
    /// @return The aspect ratio (width / height) of the window.
    float GetAspectRatio() const 
    { 
        return static_cast<float>(GetWidth()) / static_cast<float>(GetHeight());
    }

// ===========================================================
// Window mode properties
// ===========================================================

    /// @brief Sets the window to fullscreen mode.
    /// @param fullscreen True to enable fullscreen, false to disable.
    virtual void SetFullscreen(bool fullscreen) = 0;

    /// @brief Checks if the window is currently in fullscreen mode.
    virtual bool IsFullscreen() const = 0;

    /// @brief Sets the window to borderless fullscreen mode.
    /// @param enabled True to enable borderless fullscreen, false to disable.
    virtual void SetBorderlessFullscreen(bool enabled) = 0;

    /// @brief Checks if borderless fullscreen mode is enabled.
    virtual bool IsBorderlessFullscreen() const = 0;

// ====================================================================
// V-Sync
// ====================================================================

    /**
     * @brief Enable or disable vertical synchronisation (V-Sync).
     *
     * When enabled, the presented image is synchronised to the display
     * refresh rate, preventing screen tearing at the cost of input latency.
     *
     * @param enabled `true` to enable V-Sync; `false` to disable it.
     */
    virtual void SetVSync(bool enabled) = 0;

    /// @brief Checks if vertical synchronization (V-Sync) is enabled.
    virtual bool IsVSync() const = 0;

// ===========================================================
// Window title
// ===========================================================

    /// @brief Sets the title of the window.
    virtual void SetTitle(const std::string& title) = 0;

    /// @brief Retrieves the title of the window.
    virtual const std::string& GetTitle() const = 0;

    // Convenience aliases used by Scene code
    void SetWindowTitle(const std::string& title) { SetTitle(title); }
    void SetWindowSize(uint32_t w, uint32_t h)    { SetSize(w, h); }

// ===========================================================
// Native handle
// ===========================================================

    /// @brief Retrieves the underlying native window handle.
    /// @return A platform-specific pointer (e.g., GLFWwindow*, HWND, etc.).
    virtual void* GetNativeWindow() const = 0;

    /**
     * @brief Factory method to create a new window instance.
     * @param props The properties to initialize the window with.
     * @return A unique pointer to the created window instance.
     */
    static Scope<Window> Create(const WindowProps& props = WindowProps());
};

} // namespace RS
