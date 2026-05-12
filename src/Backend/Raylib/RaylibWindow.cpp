#include "rspch.hpp"

#include "Backend/Raylib/RaylibWindow.hpp"
#include "Backend/Raylib/RaylibKeyMap.hpp"
#include "Raysim/Math/Types/Vector2.hpp"

#include "Raysim/Events/Events.hpp"

#include <raylib.h>

namespace RS::Backend {

// ============================================================================
// Construction & Destruction
// ============================================================================

RaylibWindow::RaylibWindow(const WindowProps& props)
    : Window(props)
{
    RS_PROFILE_FUNCTION();

    if (props.Width == 0 || props.Height == 0) {
        RS_CORE_ERROR("Invalid window size: {}x{}. Width and height must be greater than 0.", props.Width, props.Height);
        RS_ASSERT(false, "Window size must be greater than 0");
    }

    RS_CORE_DEBUG("Initializing RaylibWindow with title '{}', size {}x{}", props.Title, props.Width, props.Height);
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
    RS_PROFILE_FUNCTION();
    RS_CORE_DEBUG("Shutting down RaylibWindow");
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
    RS_PROFILE_FUNCTION();

    if (!m_EventCallback)
        return;

    // ---- Keyboard: pressed / released / repeat ---------------------------------
    //
    // Raylib input state is refreshed by PollInputEvents(), which is called
    // internally by EndDrawing() at the end of each frame. By the time
    // ImplPollEvents() runs (start of the next frame), the state is already
    // fresh, no manual PollInputEvents() call is needed here.
    //
    // We iterate our KeyCode table (106 entries) instead of draining
    // GetKeyPressed(), so the polling API’s GetLastKeyPressed() is unaffected.

    for (std::size_t i = 1; i < KeyCodeToRaylib.size(); ++i)
    {
        const int nativeKey = KeyCodeToRaylib[i];
        if (nativeKey == 0)
            continue;

        const auto kc = static_cast<KeyCode>(i);

        if (::IsKeyPressed(nativeKey))
        {
            const bool repeat = ::IsKeyPressedRepeat(nativeKey);
            KeyPressedEvent e(kc, repeat, static_cast<ScanCode>(nativeKey));
            m_EventCallback(e);
        }

        if (::IsKeyReleased(nativeKey))
        {
            KeyReleasedEvent e(kc, static_cast<ScanCode>(nativeKey));
            m_EventCallback(e);
        }
    }

    // ---- Typed characters (Unicode codepoints) ---------------------------------
    {
        int cp;
        while ((cp = ::GetCharPressed()) != 0)
        {
            KeyTypedEvent e(static_cast<uint32_t>(cp));
            m_EventCallback(e);
        }
    }

    // ---- Mouse: movement -------------------------------------------------------
    {
        const ::Vector2 delta = ::GetMouseDelta();
        if (delta.x != 0.0f || delta.y != 0.0f)
        {
            const ::Vector2 pos = ::GetMousePosition();
            MouseMovedEvent e(pos.x, pos.y);
            m_EventCallback(e);
        }
    }

    // ---- Mouse: scroll wheel ---------------------------------------------------
    {
        const ::Vector2 scroll = ::GetMouseWheelMoveV();
        if (scroll.x != 0.0f || scroll.y != 0.0f)
        {
            MouseScrolledEvent e(scroll.x, scroll.y);
            m_EventCallback(e);
        }
    }

    // ---- Mouse: buttons --------------------------------------------------------
    // Iterate through the keymap instead of raw indices so we skip any
    // engine buttons that have no Raylib equivalent (native == -1).
    // This also guards against passing out-of-range indices to Raylib
    // (e.g. MouseButton::Button7 has no native Raylib mapping).
    for (std::size_t i = 0; i < MouseButtonToRaylib.size(); ++i)
    {
        const int native = MouseButtonToRaylib[i];
        if (native < 0)
            continue;

        if (::IsMouseButtonPressed(native))
        {
            MouseButtonPressedEvent e(static_cast<MouseButton>(i));
            m_EventCallback(e);
        }
        if (::IsMouseButtonReleased(native))
        {
            MouseButtonReleasedEvent e(static_cast<MouseButton>(i));
            m_EventCallback(e);
        }
    }

    // ---- Window: resize --------------------------------------------------------
    if (::IsWindowResized())
    {
        m_Data.Width  = ::GetScreenWidth();
        m_Data.Height = ::GetScreenHeight();
        WindowResizeEvent e(static_cast<uint32_t>(m_Data.Width),
                            static_cast<uint32_t>(m_Data.Height));
        m_EventCallback(e);
    }

    // ---- Window: focus ---------------------------------------------------------
    if (::IsWindowFocused()  && m_WasUnfocused)
    {
        m_WasUnfocused = false;
        WindowFocusEvent e(true);
        m_EventCallback(e);
    }
    else if (!::IsWindowFocused() && !m_WasUnfocused)
    {
        m_WasUnfocused = true;
        WindowFocusEvent e(false);
        m_EventCallback(e);
    }
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
    RS_PROFILE_FUNCTION();
    m_Data.Width = width;
    m_Data.Height = height;

    if (!::IsWindowReady())
        return;

    ::SetWindowSize(m_Data.Width, m_Data.Height);
    RS_CORE_DEBUG("Window size updated to {}x{}", m_Data.Width, m_Data.Height);
}

Math::Vec2i RaylibWindow::ImplGetSize() const
{
    return Math::Vec2i{m_Data.Width, m_Data.Height};
}

// ============================================================================
// Title
// ============================================================================

void RaylibWindow::ImplSetTitle(const std::string& title)
{
    RS_PROFILE_FUNCTION();
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
    RS_PROFILE_FUNCTION();
    if (fullscreen == m_Data.Fullscreen)
        return;

    m_Data.Fullscreen = fullscreen;

    ::ToggleFullscreen();

    RS_CORE_DEBUG("Changed window mode to {}", fullscreen ? "fullscreen" : "windowed");
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

} // namespace RS::Backend
