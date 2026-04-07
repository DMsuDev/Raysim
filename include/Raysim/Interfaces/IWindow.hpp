#pragma once
#include <string>

#include "Raysim/Math/Vector2.hpp"

namespace RS {

/// Initial configuration used to open a window.
struct WindowProps {
    std::string Title  = "Raysim App";
    uint32_t    Width  = 1600;
    uint32_t    Height = 900;

    WindowProps() = default;
    WindowProps(const std::string& title, uint32_t width, uint32_t height)
        : Title(title), Width(width), Height(height) {}
};

/**
 * @interface IWindow
 * @brief Abstract interface for window creation and management.
 */
class IWindow
{
public:
    virtual ~IWindow() = default;

    // -- Lifecycle -----------------------------------------------------------

    /// Open the window with the given properties. Must be called before any other method.
    virtual void Init(const WindowProps& props) = 0;

    /// Returns true when the user has requested the window to close.
    virtual bool ShouldClose() const = 0;

    // -- Setters -------------------------------------------------------------

    virtual void SetWindowTitle(const std::string& title) = 0;
    virtual void SetWindowSize(int width, int height)     = 0;
    virtual void SetWindowSize(const Vector2& size)       = 0;
    virtual void ToggleFullscreen()                       = 0;

    // -- Getters -------------------------------------------------------------

    virtual bool               IsFullscreen()   const = 0;
    virtual Vector2            GetWindowSize()  const = 0;
    virtual float              GetAspectRatio() const = 0;
    virtual uint32_t           GetWidth()       const = 0;
    virtual uint32_t           GetHeight()      const = 0;
    virtual const std::string& GetWindowTitle() const = 0;
};

} // namespace RS
