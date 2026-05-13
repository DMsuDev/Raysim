#include "rspch.hpp"

#include "Raysim/Renderer/RenderCommand.hpp"

namespace RS {

// Define the static member variable for the RendererAPI instance.
// This is defined in the .cpp file to ensure that there is only one shared instance
// across all modules (EXE and DLLs). Using `inline static` in the header would create
// separate instances in each module, leading to crashes when the API is accessed.
Scope<RendererAPI> RenderCommand::m_API;

void RenderCommand::Init(Scope<RendererAPI> api)
{
    RS_CORE_ASSERT(api != nullptr, "RendererAPI instance cannot be null");
    RS_PROFILE_FUNCTION();

    m_API = std::move(api);
    m_API->Init();
}

void RenderCommand::Shutdown()
{
    m_API.reset();
}

void RenderCommand::BeginFrame()
{
    m_API->BeginFrame();
}
void RenderCommand::EndFrame()
{
    m_API->EndFrame();
}

void RenderCommand::SetViewport(uint32_t x, uint32_t y, uint32_t width, uint32_t height)
{
    m_API->SetViewport(x, y, width, height);
}

void RenderCommand::ClearScreen(const Color& color)
{
    m_API->ClearScreen(color);
}
void RenderCommand::Clear()
{
    m_API->Clear();
}

} // namespace RS
