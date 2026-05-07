#include "pch.hpp"

#include "Raysim/Scene/Scene.hpp"
#include "Raysim/ImGui/ImGuiLayer.hpp"
#include "Raysim/Core/BackendFactory.hpp"
#include "Raysim/Core/ApplicationConfig.hpp"

namespace RS {

// ============================================================================
// ImGui frame wrappers
// ============================================================================

void Scene::ImGuiBeginFrame()
{
    if (m_ImGuiLayer)
        m_ImGuiLayer->Begin();
}

void Scene::ImGuiEndFrame()
{
    if (m_ImGuiLayer)
        m_ImGuiLayer->End();
}

// ============================================================================
// ImGui layer setup
// ============================================================================

ImGuiLayer* Scene::SetupImGuiLayer()
{
    RS_PROFILE_FUNCTION();

    RS_CORE_ASSERT(m_Context, "Scene has no EngineContext: call SetupImGuiLayer() from OnAttach() or OnStart()");
    RS_CORE_ASSERT(m_ImGuiLayer == nullptr, "ImGuiLayer already set up for this scene");

    RS_CORE_TRACE("Scene '{}': setting up ImGuiLayer", m_Config.Name);

    auto backend = BackendFactory::CreateImGuiBackend();

    auto* layer = new ImGuiLayer(GetWindow(), std::move(backend));
    m_ImGuiLayer = layer;
    m_LayerStack.PushOverlay(layer);

    RS_CORE_DEBUG("Scene '{}': ImGuiLayer ready", m_Config.Name);
    return m_ImGuiLayer;
}

} // namespace RS
