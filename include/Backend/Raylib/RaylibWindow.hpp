#pragma once

#include "Raysim/Core/Window.hpp"
#include "Raysim/Math/Vector2.hpp"

namespace RS {

/**
 * @class RaylibWindow
 * @brief Raylib implementation of the Window interface.
 *
 * Creates and manages a raylib window.
 *
 * @see Window, RaylibRendererAPI
 */
class RaylibWindow : public Window
{
public:
    RaylibWindow(const WindowProps& props);
    ~RaylibWindow() override;

// ===========================================================
// Window Methods
// ===========================================================

protected:

    void ImplPollEvents() override;
    void ImplSwapBuffers() override;

    bool ImplShouldClose() const override;

    // -- Size ----------------------------------------------------------------

    void ImplSetSize(int width, int height) override;
    Vector2Int ImplGetSize() const override;

    // -- Title ---------------------------------------------------------------

    void ImplSetTitle(const std::string& title) override;

    // -- Fullscreen ----------------------------------------------------------

    void ImplSetFullscreen(bool fullscreen) override;
    bool ImplIsFullscreen() const override { return m_Data.Fullscreen; }

    bool ImplIsMinimized() const override;

    void ImplGetFramebufferSize(int& width, int& height) const override
    {
        width = m_Data.Width;
        height = m_Data.Height;
    }

    // -- VSync ---------------------------------------------------------------

    void ImplSetVSync(bool enabled) override;
    bool ImplIsVSync() const override { return m_Data.VSync; }

    // -- Native handle ----------------------------------------------

    /// Raylib does not expose a native window handle, so this returns nullptr.
    void* ImplGetNativeWindow() const override { return nullptr; }

// ===========================================================
// Internal
// ===========================================================

private:
    void Shutdown();

    struct WindowData
    {
        std::string  Title;
        int Width  = 0;
        int Height = 0;
        bool VSync = true;
        bool Fullscreen = false;
    };

    WindowData m_Data;
};

} // namespace RS
