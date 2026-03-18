#pragma once
#include "../../Interfaces/IRenderer.hpp"

namespace RS {

/// Raylib implementation of IRenderer.
class RaylibRenderer : public IRenderer
{
public:
    RaylibRenderer()  = default;
    ~RaylibRenderer() override = default;

    // -- Frame ---------------------------------------------------------------

    void BeginFrame() override;
    void EndFrame()   override;

    // -- Clear screen --------------------------------------------------------

    void ClearScreen(const Color& color)   override;
    void ClearScreen(const Vector3& color) override;

    // -- V-Sync --------------------------------------------------------------

    void SetVSync(bool enabled) override;
    bool IsVSync()        const override;

private:
    bool vsync_ = false;
};

} // namespace RS
