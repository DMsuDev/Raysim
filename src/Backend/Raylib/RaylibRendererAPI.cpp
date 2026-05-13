#include "rspch.hpp"

#include "Raysim/Graphics/Color.hpp"
#include "Backend/Raylib/RaylibRendererAPI.hpp"

#include <raylib.h>

namespace RS::Backend {

void RaylibRendererAPI::ClearScreen(const Color& color)
{
    RS_PROFILE_FUNCTION();
    ::ClearBackground(::Color{color.r, color.g, color.b, color.a});
}

void RaylibRendererAPI::Clear()
{
    RS_PROFILE_FUNCTION();
    ::ClearBackground(::DARKGRAY);
}

void RaylibRendererAPI::BeginFrame()
{
    RS_PROFILE_FUNCTION();
    ::BeginDrawing();
}

void RaylibRendererAPI::EndFrame()
{
    RS_PROFILE_FUNCTION();
    ::EndDrawing();
}

void RaylibRendererAPI::SetViewport(uint32_t /*x*/, uint32_t /*y*/,
                                    uint32_t /*width*/, uint32_t /*height*/)
{
    RS_PROFILE_FUNCTION();
    LogOnceRegistry::LogOnce(
        "SetViewport",
        LogLevel::Warn,
        "SetViewport is not supported by the Raylib backend and will be ignored."
    );
}

} // namespace RS::Backend
