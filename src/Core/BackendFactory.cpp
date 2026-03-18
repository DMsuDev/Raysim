#include "Raysim/Core/BackendFactory.hpp"

#include "Raysim/Backend/Raylib/RaylibRenderer.hpp"
#include "Raysim/Backend/Raylib/RaylibWindow.hpp"
#include "Raysim/Backend/Raylib/RaylibInput.hpp"

namespace RS {

std::unique_ptr<IRenderer> BackendFactory::CreateRenderer(BackendType type)
{
    switch (type)
    {
        case BackendType::Raylib: return std::make_unique<RaylibRenderer>();
    }
    return nullptr;
}

std::unique_ptr<IWindow> BackendFactory::CreateWindow(BackendType type)
{
    switch (type)
    {
        case BackendType::Raylib: return std::make_unique<RaylibWindow>();
    }
    return nullptr;
}

std::unique_ptr<IInput> BackendFactory::CreateInput(BackendType type)
{
    switch (type)
    {
        case BackendType::Raylib: return std::make_unique<RaylibInput>();
    }
    return nullptr;
}

} // namespace RS
