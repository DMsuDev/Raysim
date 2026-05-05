#include "pch.hpp"

#include "Raysim/Core/BackendFactory.hpp"

#include "Backend/Raylib/RaylibRendererAPI.hpp"
#include "Backend/Raylib/RaylibWindow.hpp"
#include "Backend/Raylib/RaylibInput.hpp"
#include "Backend/Raylib/RaylibFontRenderer.hpp"

namespace RS {

Scope<RendererAPI> BackendFactory::CreateRenderer(RenderAPI api)
{
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

} // namespace RS
