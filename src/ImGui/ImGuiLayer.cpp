#include "pch.hpp"

#include "Raysim/ImGui/ImGuiLayer.hpp"
#include "Raysim/Core/Window.hpp"

#include "imgui.h"
#include "imgui_internal.h" // ImGuiContext::ActiveId

namespace RS {

ImGuiLayer::ImGuiLayer(Window& window, Scope<ImGuiBackend> backend)
    : Layer("ImGuiLayer")
    , m_Window(&window)
    , m_Backend(std::move(backend))
{
    RS_CORE_ASSERT(m_Backend != nullptr, "ImGuiLayer requires a valid ImGuiBackend");
}

// ============================================================================
// Layer Lifecycle
// ============================================================================

void ImGuiLayer::OnAttach()
{
    IMGUI_CHECKVERSION();

    m_Backend->Init(*m_Window);
    m_Initialized = true;

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;

    // Apply default visual style on the context the backend just created.
    ApplyStyle(ImGuiStyles::BuiltinStyle::Dark);

    RS_CORE_INFO("ImGuiLayer attached");
}

void ImGuiLayer::OnDetach()
{
    if (m_Initialized)
    {
        m_Backend->Shutdown();
        m_Initialized = false;
    }
    RS_CORE_INFO("ImGuiLayer detached");
}

// ============================================================================
// Rendering Commands
// ============================================================================

void ImGuiLayer::Begin()
{
    m_Backend->BeginFrame();
}

void ImGuiLayer::End()
{
    // Update display size in ImGuiIO so that it can be used by backends and UI code.
    // This is necessary in case the window was resized since the last frame.
    ImGuiIO& io = ImGui::GetIO();
    io.DisplaySize = ImVec2(
        static_cast<float>(m_Window->GetWidth()),
        static_cast<float>(m_Window->GetHeight())
    );

    m_Backend->EndFrame();
}

// ============================================================================
// Style helpers
// ============================================================================

bool ImGuiLayer::ApplyStyle(const std::string& name)
{
    if (ImGuiStyles::ApplyStyle(name))
    {
        m_CurrentStyle = name;
        return true;
    }
    return false;
}

bool ImGuiLayer::ApplyStyle(ImGuiStyles::BuiltinStyle s)
{
    if (ImGuiStyles::ApplyStyle(s))
    {
        switch (s)
        {
            case ImGuiStyles::BuiltinStyle::Classic:   m_CurrentStyle = "Classic";   break;
            case ImGuiStyles::BuiltinStyle::Dark:      m_CurrentStyle = "Dark";      break;
            case ImGuiStyles::BuiltinStyle::Light:     m_CurrentStyle = "Light";     break;
            case ImGuiStyles::BuiltinStyle::Solarized: m_CurrentStyle = "Solarized"; break;
            default:                                   m_CurrentStyle = "Unknown";   break;
        }
        return true;
    }
    return false;
}

// ============================================================================
// Utilities
// ============================================================================

uint32_t ImGuiLayer::GetActiveWidgetID() const
{
    ImGuiContext* ctx = ImGui::GetCurrentContext();
    return ctx ? ctx->ActiveId : 0u;
}

} // namespace RS
