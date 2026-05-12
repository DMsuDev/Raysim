#include "rspch.hpp"

#include "Raysim/Core/LayerStack.hpp"
#include "Raysim/Events/Event.hpp"

namespace RS {

// ========================================================================
// LIFECYCLE
// ========================================================================

LayerStack::~LayerStack()
{
    RS_CORE_TRACE("LayerStack: destroying ({} layers)", m_Layers.size());
    for (Layer* layer : m_Layers)
    {
        layer->OnDetach();
        delete layer;
    }
}

// ========================================================================
// LAYER MANAGEMENT
// ========================================================================

void LayerStack::PushLayer(Layer* layer)
{
    RS_PROFILE_FUNCTION();
    RS_CORE_ASSERT(layer != nullptr, "LayerStack::PushLayer: layer must not be null");
    RS_CORE_TRACE("LayerStack: pushing layer '{}'", layer->GetName());
    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
    m_LayerInsertIndex++;
    layer->OnAttach();
}

void LayerStack::PushOverlay(Layer* overlay)
{
    RS_PROFILE_FUNCTION();
    RS_CORE_ASSERT(overlay != nullptr, "LayerStack::PushOverlay: overlay must not be null");
    RS_CORE_TRACE("LayerStack: pushing overlay '{}'", overlay->GetName());
    m_Layers.emplace_back(overlay);
    overlay->OnAttach();
}

void LayerStack::PopLayer(Layer* layer)
{
    RS_PROFILE_FUNCTION();
    RS_CORE_ASSERT(layer != nullptr, "LayerStack::PopLayer: layer must not be null");
    auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
    if (it != m_Layers.begin() + m_LayerInsertIndex)
    {
        RS_CORE_TRACE("LayerStack: popping layer '{}'", layer->GetName());
        layer->OnDetach();
        m_Layers.erase(it);
        m_LayerInsertIndex--;
    }
    else
    {
        RS_CORE_WARN("LayerStack::PopLayer: layer '{}' not found", layer->GetName());
    }
}

void LayerStack::PopOverlay(Layer* overlay)
{
    RS_PROFILE_FUNCTION();
    RS_CORE_ASSERT(overlay != nullptr, "LayerStack::PopOverlay: overlay must not be null");
    auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
    if (it != m_Layers.end())
    {
        RS_CORE_TRACE("LayerStack: popping overlay '{}'", overlay->GetName());
        overlay->OnDetach();
        m_Layers.erase(it);
    }
    else
    {
        RS_CORE_WARN("LayerStack::PopOverlay: overlay '{}' not found", overlay->GetName());
    }
}

// ========================================================================
// BATCH OPERATIONS
// ========================================================================

void LayerStack::UpdateAll(float dt)
{
    RS_PROFILE_FUNCTION();
    for (Layer* layer : m_Layers)
        layer->OnUpdate(dt);
}

void LayerStack::RenderUIAll()
{
    RS_PROFILE_FUNCTION();
    for (Layer* layer : m_Layers)
        layer->OnUIRender();
}

void LayerStack::DispatchEvent(Event& e)
{
    RS_PROFILE_FUNCTION();
    // Propagate in reverse order: overlays first, then layers beneath.
    // Stops as soon as a layer marks the event as Handled.
    for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
    {
        if (e.Handled)
            break;
        (*it)->OnEvent(e);
    }
}

void LayerStack::Clear()
{
    RS_PROFILE_FUNCTION();
    RS_CORE_TRACE("LayerStack: clearing {} layer(s)", m_Layers.size());
    for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
    {
        (*it)->OnDetach();
        delete *it;
    }
    m_Layers.clear();
    m_LayerInsertIndex = 0;
}

} // namespace RS
