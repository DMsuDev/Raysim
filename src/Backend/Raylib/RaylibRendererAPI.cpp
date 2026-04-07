#include "../../pch.hpp"

#include "Raysim/Graphics/Color.hpp"
#include "Raysim/Math/Vector3.hpp"

#include "Raysim/Backend/Raylib/RaylibRendererAPI.hpp"

#include <raylib.h>

namespace RS {

void RaylibRendererAPI::ClearScreen(const Color& color)
{
    ::ClearBackground(::Color{color.r, color.g, color.b, color.a});
}

void RaylibRendererAPI::ClearScreen(const Vector3& color)
{
    auto toByte = [](float v) -> unsigned char {
        float clamped = (v < 0.0f) ? 0.0f : (v > 1.0f) ? 1.0f : v;
        return static_cast<unsigned char>(clamped * 255.0f + 0.5f);
    };
    ::ClearBackground(::Color{toByte(color.x), toByte(color.y), toByte(color.z), 255});
}

void RaylibRendererAPI::Clear()
{
    ::ClearBackground(::DARKGRAY);
}

void RaylibRendererAPI::BeginFrame()
{
    ::BeginDrawing();
}

void RaylibRendererAPI::EndFrame()
{
    ::EndDrawing();
}

void RaylibRendererAPI::SetViewport(uint32_t /*x*/, uint32_t /*y*/,
                                    uint32_t /*width*/, uint32_t /*height*/)
{
    // Raylib does not expose a direct viewport call;
    // the viewport matches the window size automatically.
}

void RaylibRendererAPI::SetVSync(bool enabled)
{
    if (enabled) ::SetConfigFlags(FLAG_VSYNC_HINT);
    else         ::ClearWindowState(FLAG_VSYNC_HINT);
    m_vsync = enabled;
}

bool RaylibRendererAPI::IsVSync() const
{
    return m_vsync;
}

} // namespace RS
