#include "../../pch.hpp"

#include "Raysim/Graphics/Color.hpp"
#include "Raysim/Math/Vector3.hpp"

#include "Raysim/Backend/Raylib/RaylibRenderer.hpp"

#include <raylib.h>

namespace RS {

void RaylibRenderer::ClearScreen(const Color& color)
{
    ::ClearBackground(::Color{color.r, color.g, color.b, color.a});
}

void RaylibRenderer::ClearScreen(const Vector3& color)
{
    auto toByte = [](float v) -> unsigned char {
        float clamped = (v < 0.0f) ? 0.0f : (v > 1.0f) ? 1.0f : v;
        return static_cast<unsigned char>(clamped * 255.0f + 0.5f);
    };
    ::ClearBackground(::Color{toByte(color.x), toByte(color.y), toByte(color.z), 255});
}

void RaylibRenderer::BeginFrame()
{
    ::BeginDrawing();
}

void RaylibRenderer::EndFrame()
{
    ::EndDrawing();
}

void RaylibRenderer::SetVSync(bool enabled)
{
    if (enabled) ::SetConfigFlags(FLAG_VSYNC_HINT);
    else         ::ClearWindowState(FLAG_VSYNC_HINT);
    vsync_ = enabled;
}

bool RaylibRenderer::IsVSync() const
{
    return vsync_;
}

} // namespace RS
