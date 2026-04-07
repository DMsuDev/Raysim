#include "Raysim/Core/BackendFactory.hpp"

#include "Raysim/Backend/Raylib/RaylibRendererAPI.hpp"
#include "Raysim/Backend/Raylib/RaylibWindow.hpp"
#include "Raysim/Backend/Raylib/RaylibInput.hpp"

namespace RS {

std::unique_ptr<RendererAPI> BackendFactory::CreateRenderer(RenderAPI api)
{
    switch (api)
    {
        case RenderAPI::Raylib: return std::make_unique<RaylibRendererAPI>();
        default: return nullptr;
    }
}

std::unique_ptr<IWindow> BackendFactory::CreateWindow(WindowBackend backend)
{
    switch (backend)
    {
        case WindowBackend::Raylib: return std::make_unique<RaylibWindow>();
        default: return nullptr;
    }
}

std::unique_ptr<Input> BackendFactory::CreateInput(WindowBackend backend)
{
    switch (backend)
    {
        case WindowBackend::Raylib: return std::make_unique<RaylibInput>();
        default: return nullptr;
    }
}

} // namespace RS
