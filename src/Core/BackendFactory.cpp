#include "pch.hpp"

#include "Raysim/Core/BackendFactory.hpp"

#include "Backend/Raylib/RaylibRendererAPI.hpp"
#include "Backend/Raylib/RaylibWindow.hpp"
#include "Backend/Raylib/RaylibInput.hpp"
#include "Backend/Raylib/RaylibFontRenderer.hpp"
#include "Backend/Raylib/RaylibImGuiBackend.hpp"

namespace RS {

Scope<RendererAPI> BackendFactory::CreateRenderer(RenderAPI api)
{
    RS_PROFILE_FUNCTION();
    switch (api)
    {
        case RenderAPI::Raylib:
        {
            RS_CORE_DEBUG("Renderer backend created: Raylib");
            return CreateScope<Backend::RaylibRendererAPI>();
        }
        default:
        {
            RS_CORE_ERROR("Unsupported renderer backend: {}", static_cast<int>(api));
            return nullptr;
        }
    }
}

Scope<Window> BackendFactory::CreateAppWindow(WindowBackend backend, const WindowProps& props)
{
    RS_PROFILE_FUNCTION();
    switch (backend)
    {
        case WindowBackend::Raylib:
        {
            RS_CORE_DEBUG("Window backend created: Raylib");
            return CreateScope<Backend::RaylibWindow>(props);
        }
        default:
        {
            RS_CORE_ERROR("Unsupported window backend: {}", static_cast<int>(backend));
            return nullptr;
        }
    }

    return nullptr;
}


Scope<Input> BackendFactory::CreateInput(WindowBackend backend)
{
    RS_PROFILE_FUNCTION();
    switch (backend)
    {
        case WindowBackend::Raylib:
        {
            RS_CORE_DEBUG("Input backend created: Raylib");
            return CreateScope<Backend::RaylibInput>();
        }

        default:
        {
            RS_CORE_ERROR("Unsupported input backend: {}", static_cast<int>(backend));
            return nullptr;
        }
    }
}

Scope<Fonts::FontRenderer> BackendFactory::CreateFontRenderer(RenderAPI api)
{
    RS_PROFILE_FUNCTION();
    switch (api)
    {
        case RenderAPI::Raylib:
        {
            RS_CORE_DEBUG("Font renderer backend created: Raylib");
            return CreateScope<Backend::RaylibFontRenderer>();
        }
        default:
        {
            RS_CORE_ERROR("Unsupported font renderer backend: {}", static_cast<int>(api));
            return nullptr;
        }
    }
}

Scope<ImGuiBackend> BackendFactory::CreateImGuiBackend(WindowBackend windowBackend, RenderAPI renderAPI)
{
    RS_PROFILE_FUNCTION();
    switch (windowBackend)
    {
        case WindowBackend::Raylib:
        {
            RS_CORE_DEBUG("ImGui backend created: Raylib (rlImGui stub)");
            return CreateScope<Backend::RaylibImGuiBackend>();
        }

        default:
        {
            RS_CORE_ERROR("Unsupported ImGui backend — WindowBackend: {}, RenderAPI: {}",
                          static_cast<int>(windowBackend), static_cast<int>(renderAPI));
            return nullptr;
        }
    }
}

} // namespace RS
