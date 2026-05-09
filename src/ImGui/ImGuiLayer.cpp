#include "rspch.hpp"

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
    RS_PROFILE_FUNCTION();

    RS_CORE_ASSERT(!m_Initialized, "ImGuiLayer::OnAttach called but layer is already initialized");

    IMGUI_CHECKVERSION();

    RS_CORE_TRACE("ImGuiLayer: initializing backend");
    m_Backend->Init(*m_Window);
    m_Initialized = true;

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
    io.IniFilename = nullptr; // Disable saving/loading of ImGui settings to/from .ini file

    // Apply default visual style on the context the backend just created.
    ApplyStyle(ImGuiStyles::BuiltinStyle::Dark);

    RS_CORE_DEBUG("ImGuiLayer attached");
}

void ImGuiLayer::OnDetach()
{
    RS_PROFILE_FUNCTION();

    if (m_Initialized)
    {
        RS_CORE_TRACE("ImGuiLayer: shutting down backend");
        m_Backend->Shutdown();
        m_Initialized = false;
    }
    else
    {
        RS_CORE_WARN("ImGuiLayer::OnDetach called but layer was not initialized");
    }
    RS_CORE_DEBUG("ImGuiLayer detached");
}

// ============================================================================
// Rendering Commands
// ============================================================================

void ImGuiLayer::Begin()
{
    RS_PROFILE_FUNCTION();

    RS_CORE_ASSERT(m_Initialized, "ImGuiLayer::Begin called before OnAttach");
    m_Backend->BeginFrame();
}

void ImGuiLayer::End()
{
    RS_PROFILE_FUNCTION();

    RS_CORE_ASSERT(m_Initialized, "ImGuiLayer::End called before OnAttach");

    // Update display size in ImGuiIO so that it can be used by backends and UI code.
    // This is necessary in case the window was resized since the last frame.
    ImGuiIO& io = ImGui::GetIO();
    const float displayW = static_cast<float>(m_Window->GetWidth());
    const float displayH = static_cast<float>(m_Window->GetHeight());
    RS_CORE_ASSERT(displayW > 0.0f && displayH > 0.0f, "ImGuiLayer::End: window has zero or negative display size");
    io.DisplaySize = ImVec2(displayW, displayH);

    m_Backend->EndFrame();
}

// ============================================================================
// Style helpers
// ============================================================================

bool ImGuiLayer::ApplyStyle(const std::string& name)
{
    RS_PROFILE_FUNCTION();

    RS_CORE_ASSERT(!name.empty(), "ImGuiLayer::ApplyStyle: style name must not be empty");
    RS_CORE_TRACE("ImGuiLayer: applying style '{}'", name);

    if (ImGuiStyles::ApplyStyle(name))
    {
        m_CurrentStyle = name;
        RS_CORE_DEBUG("ImGuiLayer: style '{}' applied", name);
        return true;
    }

    RS_CORE_WARN("ImGuiLayer: style '{}' not found", name);
    return false;
}

bool ImGuiLayer::ApplyStyle(ImGuiStyles::BuiltinStyle s)
{
    RS_PROFILE_FUNCTION();

    RS_CORE_TRACE("ImGuiLayer: applying builtin style (id={})", static_cast<int>(s));

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
        RS_CORE_DEBUG("ImGuiLayer: builtin style '{}' applied", m_CurrentStyle);
        return true;
    }

    RS_CORE_WARN("ImGuiLayer: builtin style (id={}) could not be applied", static_cast<int>(s));
    return false;
}

// ============================================================================
// Utilities
// ============================================================================

uint32_t ImGuiLayer::GetActiveWidgetID() const
{
    RS_CORE_ASSERT(m_Initialized, "ImGuiLayer::GetActiveWidgetID called before OnAttach");
    ImGuiContext* ctx = ImGui::GetCurrentContext();
    if (!ctx)
    {
        RS_CORE_WARN("ImGuiLayer::GetActiveWidgetID: no active ImGui context");
        return 0u;
    }
    return ctx->ActiveId;
}

} // namespace RS
