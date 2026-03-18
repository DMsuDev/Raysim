#include "../../pch.hpp"

#include "Raysim/Math/Vector2.hpp"
#include "Raysim/Backend/Raylib/RaylibWindow.hpp"

#include <raylib.h>

namespace RS {

// ===============================
// Lifecycle
// ===============================

void RaylibWindow::Init(const WindowProps& props)
{
    if (props.Width == 0 || props.Height == 0) {
        LOG_ERROR("Invalid window size: {}x{}. Width and height must be greater than 0.", props.Width, props.Height);
        RS_ASSERT(false, "Window size must be greater than 0");
    }

    LOG_INFO("Initializing RaylibWindow with title '{}', size {}x{}", props.Title, props.Width, props.Height);
    title_  = props.Title;
    width_  = static_cast<int>(props.Width);
    height_ = static_cast<int>(props.Height);
    prevWidth_  = width_;
    prevHeight_ = height_;

    ::SetTraceLogLevel(LOG_ERROR);
    ::InitWindow(width_, height_, title_.c_str());
    LOG_DEBUG("RaylibWindow ready: title='{}', {}x{}", title_, width_, height_);
}

bool RaylibWindow::ShouldClose() const
{
    return ::WindowShouldClose();
}

void RaylibWindow::SetWindowTitle(const std::string& title)
{
    LOG_INFO("Setting window title to '{}'", title);
    RS_ASSERT(!title.empty(), "Window title cannot be empty");

    title_ = title;

    if (::IsWindowReady())
    {
        ::SetWindowTitle(title_.c_str());
        LOG_DEBUG("Window title updated to '{}'", title_);
    }
}

void RaylibWindow::SetWindowSize(int width, int height)
{
    LOG_INFO("Setting window size to {}x{}", width, height);
    RS_ASSERT(width > 0 && height > 0, "Window width and height must be greater than 0");

    width_ = width;
    height_ = height;

    if (::IsWindowReady())
    {
        ::SetWindowSize(width_, height_);
        LOG_DEBUG("Window size updated to {}x{}", width_, height_);
    }
}

void RaylibWindow::SetWindowSize(const Vector2& size)
{
    LOG_INFO("Setting window size to {}x{}", size.x, size.y);
    RS_ASSERT(size.x > 0 && size.y > 0, "Window width and height must be greater than 0");

    width_ = static_cast<int>(size.x);
    height_ = static_cast<int>(size.y);

    if (!isFullscreen_) {
        prevWidth_  = width_;
        prevHeight_ = height_;
    }

    if (::IsWindowReady())
    {
        ::SetWindowSize(width_, height_);
        LOG_DEBUG("Window size updated to {}x{}", width_, height_);
    }
}

void RaylibWindow::ToggleFullscreen()
{
    if (!::IsWindowReady()) return;

    if (!isFullscreen_) {
        prevWidth_  = width_;
        prevHeight_ = height_;
    }

    ::ToggleFullscreen();
    isFullscreen_ = ::IsWindowFullscreen();

    if (isFullscreen_) {
        width_  = static_cast<int>(GetScreenWidth());
        height_ = static_cast<int>(GetScreenHeight());
    } else {
        width_  = prevWidth_;
        height_ = prevHeight_;
    }

    LOG_INFO("Changed window mode to {} ({}x{})", isFullscreen_ ? "fullscreen" : "windowed", width_, height_);
}

// ===============================
// Internal methods
// ===============================

void RaylibWindow::Shutdown()
{
    LOG_INFO("Shutting down RaylibWindow");
    if (::IsWindowReady())
    {
        ::CloseWindow();
        LOG_DEBUG("RaylibWindow closed successfully");
    }
    else LOG_WARN("Attempted to close RaylibWindow, but it was not ready");
}

} // namespace RS
