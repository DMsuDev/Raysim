#include "pch.hpp"

#include "Raysim/Math/Vector2.hpp"
#include "Backend/Raylib/RaylibWindow.hpp"

#include <raylib.h>

namespace RS {

// ============================================================================
// Construction & Destruction
// ============================================================================

RaylibWindow::RaylibWindow(const WindowProps& props)
    : Window(props)
{
    if (props.Width == 0 || props.Height == 0) {
        RS_CORE_ERROR("Invalid window size: {}x{}. Width and height must be greater than 0.", props.Width, props.Height);
        RS_ASSERT(false, "Window size must be greater than 0");
    }

    RS_CORE_INFO("Initializing RaylibWindow with title '{}', size {}x{}", props.Title, props.Width, props.Height);
    m_Data.Title  = props.Title;
    m_Data.Width  = props.Width;
    m_Data.Height = props.Height;

    ::SetConfigFlags(FLAG_WINDOW_ALWAYS_RUN);
    ::SetTraceLogLevel(LOG_ERROR);
    ::InitWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str());
    RS_CORE_DEBUG("RaylibWindow ready: title='{}', {}x{}", m_Data.Title, m_Data.Width, m_Data.Height);
}

RaylibWindow::~RaylibWindow()
{
    this->Shutdown();
}

void RaylibWindow::Shutdown()
{
    RS_CORE_INFO("Shutting down RaylibWindow");
    if (::IsWindowReady())
    {
        ::CloseWindow();
        RS_CORE_DEBUG("RaylibWindow closed successfully");
    }
    else RS_CORE_WARN("Attempted to close RaylibWindow, but it was not ready");
}

bool RaylibWindow::ImplShouldClose() const
{
    return ::WindowShouldClose();
}

// ============================================================================
// Lifecycle
// ============================================================================

void RaylibWindow::ImplPollEvents()
{
    LogOnceRegistry::LogOnce(
        "EventPolling",
        LogLevel::Warn,
        "Event polling is handled internally by Raylib, so this will be ignored"
    );
}

void RaylibWindow::ImplSwapBuffers()
{
    LogOnceRegistry::LogOnce(
        "BufferSwapping",
        LogLevel::Warn,
        "Buffer swapping is handled internally by Raylib, so this will be ignored"
    );
}

// ============================================================================
// Size
// ============================================================================

void RaylibWindow::ImplSetSize(int width, int height)
{
    m_Data.Width = width;
    m_Data.Height = height;

    if (!::IsWindowReady())
        return;

    ::SetWindowSize(m_Data.Width, m_Data.Height);
    RS_CORE_DEBUG("Window size updated to {}x{}", m_Data.Width, m_Data.Height);
}

Vector2Int RaylibWindow::ImplGetSize() const
{
    return Vector2Int{m_Data.Width, m_Data.Height};
}

// ============================================================================
// Title
// ============================================================================

void RaylibWindow::ImplSetTitle(const std::string& title)
{
    m_Data.Title = title;

    if (::IsWindowReady())
    {
        ::SetWindowTitle(m_Data.Title.c_str());
        RS_CORE_DEBUG("Window title updated to '{}'", m_Data.Title);
    }
}

// ============================================================================
// Fullscreen
// ============================================================================

void RaylibWindow::ImplSetFullscreen(bool fullscreen)
{
    if (fullscreen == m_Data.Fullscreen)
        return;

    m_Data.Fullscreen = fullscreen;

    ::ToggleFullscreen();

    RS_CORE_INFO("Changed window mode to {}", fullscreen ? "fullscreen" : "windowed");
}

bool RaylibWindow::ImplIsMinimized() const
{
    return ::IsWindowMinimized();
}

// ============================================================================
// Vsync
// ============================================================================

void RaylibWindow::ImplSetVSync(bool enabled)
{
    m_Data.VSync = enabled;
    LogOnceRegistry::LogOnce(
        "VSyncControl",
        LogLevel::Warn,
        "V-Sync control is not implemented yet, so this will be ignored"
    );
}

} // namespace RS
