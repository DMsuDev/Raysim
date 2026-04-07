#pragma once

#include "Raysim/Core/Memory.hpp"
#include "Raysim/Core/Assert.hpp"
#include "Raysim/Renderer/RendererAPI.hpp"

namespace RS {

/**
 * @class RenderCommand
 * @brief Thin command dispatcher that forwards rendering calls to the active RendererAPI.
 *
 * Provides a convenient facade so that user code can issue render commands
 * without a direct dependency on any concrete graphics API.
 * Holds a **shared** reference to the RendererAPI so that the backend
 * stays alive as long as any RenderCommand instance exists.
 *
 * Typical setup in Application:
 * @code
 * auto api = BackendFactory::CreateRenderer(RenderAPI::Raylib);
 * auto rc  = RenderCommand(Shared<RendererAPI>(std::move(api)));
 * @endcode
 *
 * @see RendererAPI
 */
class RenderCommand
{
public:
    explicit RenderCommand(Shared<RendererAPI> api)
        : m_API(std::move(api))
    {
        RS_CORE_ASSERT(m_API, "RenderCommand: RendererAPI must not be null");
    }

// ====================================================================
// Frame management
// ====================================================================

    void BeginFrame() { m_API->BeginFrame(); }
    void EndFrame()   { m_API->EndFrame();   }

// ====================================================================
// Viewport
// ====================================================================

    void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
    {
        m_API->SetViewport(x, y, width, height);
    }

// ====================================================================
// Screen clear
// ====================================================================

    void ClearScreen(const Color& color)   { m_API->ClearScreen(color); }
    void ClearScreen(const Vector3& color) { m_API->ClearScreen(color); }
    void Clear()                           { m_API->Clear();             }

// ====================================================================
// V-Sync
// ====================================================================

    void SetVSync(bool enabled) { m_API->SetVSync(enabled);  }
    bool IsVSync()        const { return m_API->IsVSync();   }

// ====================================================================
// Introspection
// ====================================================================

    /// Returns the shared RendererAPI.
    Shared<RendererAPI> GetAPI() const { return m_API; }

private:
    Shared<RendererAPI> m_API;
};

} // namespace RS
