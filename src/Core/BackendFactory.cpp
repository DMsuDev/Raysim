#include "rspch.hpp"

#include "Raysim/Core/BackendFactory.hpp"

#include "Backend/Raylib/RaylibRendererAPI.hpp"
#include "Backend/Raylib/RaylibWindow.hpp"
#include "Backend/Raylib/RaylibInput.hpp"
#include "Backend/Raylib/RaylibFontRenderer.hpp"
#include "Backend/Raylib/RaylibImGuiBackend.hpp"

namespace RS {

Scope<RendererAPI> BackendFactory::CreateRenderer()
{
    RS_PROFILE_FUNCTION();

#if defined(RS_BACKEND_RAYLIB)
    RS_CORE_DEBUG("Renderer backend created: Raylib");
    return CreateScope<Backend::RaylibRendererAPI>();
#else
    RS_CORE_ERROR("No supported renderer backend defined.");
    return nullptr;
#endif
}

Scope<Window> BackendFactory::CreateAppWindow(const WindowProps& props)
{
    RS_PROFILE_FUNCTION();

#if defined(RS_BACKEND_RAYLIB)
    RS_CORE_DEBUG("Window backend created: Raylib");
    return CreateScope<Backend::RaylibWindow>(props);
#else
    RS_CORE_ERROR("No supported window backend defined.");
    return nullptr;
#endif
}


Scope<Input> BackendFactory::CreateInput()
{
    RS_PROFILE_FUNCTION();

#if defined(RS_BACKEND_RAYLIB)
    RS_CORE_DEBUG("Input backend created: Raylib");
    return CreateScope<Backend::RaylibInput>();
#else
    RS_CORE_ERROR("No supported input backend defined.");
    return nullptr;
#endif
}

Scope<Fonts::FontRenderer> BackendFactory::CreateFontRenderer()
{
    RS_PROFILE_FUNCTION();

#if defined(RS_BACKEND_RAYLIB)
    RS_CORE_DEBUG("FontRenderer backend created: Raylib");
    return CreateScope<Backend::RaylibFontRenderer>();
#else
    RS_CORE_ERROR("No supported FontRenderer backend defined.");
    return nullptr;
#endif
}

Scope<ImGuiBackend> BackendFactory::CreateImGuiBackend()
{
    RS_PROFILE_FUNCTION();

#if defined(RS_BACKEND_RAYLIB)
    RS_CORE_DEBUG("ImGuiBackend created: Raylib");
    return CreateScope<Backend::RaylibImGuiBackend>();
#else
    RS_CORE_ERROR("No supported ImGuiBackend defined for the current window and renderer backends.");
    return nullptr;
#endif
}

} // namespace RS
