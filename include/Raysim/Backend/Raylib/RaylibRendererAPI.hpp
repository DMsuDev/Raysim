#pragma once
#include "Raysim/Renderer/RendererAPI.hpp"

namespace RS {

/**
 * @class RaylibRendererAPI
 * @brief Raylib implementation of the `RendererAPI` interface.
 *
 * Implements the core rendering operations using raylib's graphics functions.
 * 
 * @see RendererAPI
 */
class RaylibRendererAPI : public RendererAPI
{
public:
    void BeginFrame() override;
    void EndFrame()   override;

    void SetViewport(uint32_t x, uint32_t y,
                     uint32_t width, uint32_t height) override;

    void ClearScreen(const Color& color)   override;
    /// Overload to allow clearing with a linear RGB color vector [0,1]
    void ClearScreen(const Vector3& color) override;
    void Clear() override;
    
    void SetVSync(bool enabled) override;
    bool IsVSync()        const override;

private:
    bool m_vsync = false;
};

} // namespace RS
