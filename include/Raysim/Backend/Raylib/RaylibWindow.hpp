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

    /// Raylib handles window updates internally, so this is a no-op.
    void OnUpdate() override { }

    bool ShouldClose() const override;

    // -- Size ----------------------------------------------------------------

    void SetSize(int width, int height) override;
    void SetSize(const Vector2& size) override;

    Vector2  GetSize()   const override;
    int GetWidth()  const override { return m_Data.Width; }
    int GetHeight() const override { return m_Data.Height; }

    // -- Title ---------------------------------------------------------------

    void SetTitle(const std::string& title) override;
    const std::string& GetTitle() const override { return m_Data.Title; }

    // -- Fullscreen ----------------------------------------------------------

    void SetFullscreen(bool fullscreen) override;
    bool IsFullscreen() const override { return m_Data.Fullscreen; }

    void SetBorderlessFullscreen(bool enabled) override;
    bool IsBorderlessFullscreen() const override { return m_Data.BorderlessFullscreen; }

    bool IsMinimized() const override;

    // -- VSync ---------------------------------------------------------------

    void SetVSync(bool enabled) override;
    bool IsVSync() const override { return m_Data.VSync; }

    // -- Native handle ----------------------------------------------

    /// Raylib does not expose a native window handle, so this returns nullptr.
    void* GetNativeWindow() const override { return nullptr; }

private:
    void Init(const WindowProps& props);
    void Shutdown();

private:

    struct WindowData
    {
        std::string  Title;
        int Width  = 0;
        int Height = 0;
        bool VSync = true;
        bool Fullscreen = false;
        bool BorderlessFullscreen = false;

        // Saved windowed-mode dimensions for fullscreen toggle
        int PrevWidth  = 0;
        int PrevHeight = 0;
        int PrevPosX   = 0;
        int PrevPosY   = 0;
    };

    WindowData m_Data;
};

} // namespace RS
