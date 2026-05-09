#pragma once

#include "Raysim/Renderer/RendererAPI.hpp"

namespace RS
{
    /**
     * @class RenderCommand
     * @brief Thin command dispatcher that forwards rendering calls to the active RendererAPI.
     *
     * Provides a convenient facade so that user code can issue render commands
     * without a direct dependency on any concrete graphics API.
     * The RendererAPI instance is owned by RenderCommand and is initialized via Init().
     *
     * @see RendererAPI
     */
    class RenderCommand
    {
    public:
        static void Init(Scope<RendererAPI> api)
        {
            RS_CORE_ASSERT(api != nullptr, "RendererAPI instance cannot be null");
            RS_PROFILE_FUNCTION();

            m_API = std::move(api);
            m_API->Init();
        }
        static void Shutdown() { m_API.reset(); }

        static void BeginFrame() { m_API->BeginFrame(); }
        static void EndFrame() { m_API->EndFrame(); }

        static void SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
        {
            m_API->SetViewport(x, y, width, height);
        }

        static void ClearScreen(const Color &color) { m_API->ClearScreen(color); }
        static void ClearScreen(const Math::Vec3f &color) { m_API->ClearScreen(color); }
        static void Clear() { m_API->Clear(); }

    private:
        inline static Scope<RendererAPI> m_API;
    };

} // namespace RS
