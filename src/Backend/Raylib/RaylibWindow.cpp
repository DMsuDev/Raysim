#include "../../pch.hpp"

#include "Raysim/Math/Vector2.hpp"
#include "Raysim/Backend/Raylib/RaylibWindow.hpp"

#include <raylib.h>

namespace RS {

// ============================================================================
// Lifecycle
// ============================================================================

RaylibWindow::RaylibWindow(const WindowProps& props)
{
    this->Init(props);
}

RaylibWindow::~RaylibWindow()
{
    this->Shutdown();
}

void RaylibWindow::Init(const WindowProps& props)
{
    if (props.Width == 0 || props.Height == 0) {
        RS_CORE_ERROR("Invalid window size: {}x{}. Width and height must be greater than 0.", props.Width, props.Height);
        RS_ASSERT(false, "Window size must be greater than 0");
    }

    RS_CORE_INFO("Initializing RaylibWindow with title '{}', size {}x{}", props.Title, props.Width, props.Height);
    m_Data.Title  = props.Title;
    m_Data.Width  = props.Width;
    m_Data.Height = props.Height;
    m_Data.PrevWidth  = m_Data.Width;
    m_Data.PrevHeight = m_Data.Height;

    ::SetTraceLogLevel(LOG_ERROR);
    ::InitWindow(m_Data.Width, m_Data.Height, m_Data.Title.c_str());
    RS_CORE_DEBUG("RaylibWindow ready: title='{}', {}x{}", m_Data.Title, m_Data.Width, m_Data.Height);
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

bool RaylibWindow::ShouldClose() const
{
    return ::WindowShouldClose();
}

// ============================================================================
// Size
// ============================================================================

void RaylibWindow::SetSize(int width, int height)
{
    RS_CORE_INFO("Setting window size to {}x{}", width, height);
    RS_ASSERT(width > 0 && height > 0, "Window width and height must be greater than 0");

    m_Data.Width = width;
    m_Data.Height = height;

    if (!m_Data.Fullscreen) {
        m_Data.PrevWidth  = width;
        m_Data.PrevHeight = height;
    }

    if (::IsWindowReady())
    {
        ::SetWindowSize(m_Data.Width, m_Data.Height);
        RS_CORE_DEBUG("Window size updated to {}x{}", m_Data.Width, m_Data.Height);
    }
}

void RaylibWindow::SetSize(const Vector2& size)
{
    this->SetSize(static_cast<int>(size.x), static_cast<int>(size.y));
}

Vector2 RaylibWindow::GetSize() const
{
    return Vector2{static_cast<float>(m_Data.Width), static_cast<float>(m_Data.Height)};
}

// ============================================================================
// Title
// ============================================================================

void RaylibWindow::SetTitle(const std::string& title)
{
    RS_CORE_INFO("Setting window title to '{}'", title);
    RS_ASSERT(!title.empty(), "Window title cannot be empty");

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

void RaylibWindow::SetFullscreen(bool fullscreen)
{
    if (!::IsWindowReady()) return;

    if (fullscreen == m_Data.Fullscreen) return;

    m_Data.Fullscreen = fullscreen;
    m_Data.BorderlessFullscreen = false;

    if (fullscreen)
    {
        // Save current windowed position & size
        m_Data.PrevWidth  = m_Data.Width;
        m_Data.PrevHeight = m_Data.Height;

        this->SetSize(::GetMonitorWidth(0), ::GetMonitorHeight(0));

        ::ToggleFullscreen();
    }
    else
    {
        this->SetSize(m_Data.PrevWidth, m_Data.PrevHeight);

        // Raylib does not support programmatically exiting fullscreen mode,
        // so we simply toggle it again to return to windowed mode.
        ::ToggleFullscreen();
    }

    RS_CORE_INFO("Changed window mode to {} ({}x{})", m_Data.Fullscreen ? "fullscreen" : "windowed", m_Data.Width, m_Data.Height);
}

void RaylibWindow::SetBorderlessFullscreen(bool /*enabled*/)
{
    RS_CORE_WARN("Borderless fullscreen is not implemented in RaylibWindow, so this will be ignored");
}

bool RaylibWindow::IsMinimized() const
{
    return ::IsWindowState(FLAG_WINDOW_MINIMIZED);
}

// ============================================================================
// VSync
// ============================================================================

void RaylibWindow::SetVSync(bool /*enabled*/)
{
    RS_CORE_WARN("V-Sync control is not implemented yet, so this will be ignored");
}

} // namespace RS
