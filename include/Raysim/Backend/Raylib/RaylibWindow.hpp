#pragma once

#include "../../Interfaces/IWindow.hpp"

namespace RS {

class RaylibWindow : public IWindow
{
public:
    RaylibWindow() = default;
    ~RaylibWindow() override { Shutdown(); };

    // Lifecycle

    void Init(const WindowProps& props) override;
    bool ShouldClose() const override;

    // Window properties setters

    void SetWindowTitle(const std::string& title) override;
    void SetWindowSize(int width, int height) override;
    void SetWindowSize(const Vector2& size) override;
    void ToggleFullscreen() override;
    bool IsFullscreen() const override { return isFullscreen_; }

    // Window properties getters

    inline Vector2 GetWindowSize() const override { return Vector2{static_cast<float>(width_), static_cast<float>(height_)}; }
    inline float GetAspectRatio() const override { return static_cast<float>(width_) / static_cast<float>(height_); }
    inline uint32_t GetWidth() const override { return static_cast<uint32_t>(width_); }
    inline uint32_t GetHeight() const override { return static_cast<uint32_t>(height_); }
    inline const std::string& GetWindowTitle() const override { return title_; }

    // Event handlers
    // Empty for now, implemented later if needed

private:
    // ===============================
    // Internal methods
    // ===============================

    /**
    * @brief Shutdown the window and release resources
    *
    * Called by the destructor to clean up any resources associated with the window.
    */
    void Shutdown();

    // ===============================
    // Internal parameters and state
    // ===============================

    int width_      = 0;
    int height_     = 0;
    int prevWidth_  = 0;
    int prevHeight_ = 0;
    std::string title_;
    bool isFullscreen_ = false;
};

} // namespace RS
