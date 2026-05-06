#include "pch.hpp"
#include "Raysim/Core/LayerStack.hpp"

namespace RS {

// ========================================================================
// LIFECYCLE
// ========================================================================

LayerStack::~LayerStack()
{
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
    m_Layers.emplace(m_Layers.begin() + m_LayerInsertIndex, layer);
    m_LayerInsertIndex++;
    layer->OnAttach();
}

void LayerStack::PushOverlay(Layer* overlay)
{
    m_Layers.emplace_back(overlay);
    overlay->OnAttach();
}

void LayerStack::PopLayer(Layer* layer)
{
    auto it = std::find(m_Layers.begin(), m_Layers.begin() + m_LayerInsertIndex, layer);
    if (it != m_Layers.begin() + m_LayerInsertIndex)
    {
        layer->OnDetach();
        m_Layers.erase(it);
        m_LayerInsertIndex--;
    }
}

void LayerStack::PopOverlay(Layer* overlay)
{
    auto it = std::find(m_Layers.begin() + m_LayerInsertIndex, m_Layers.end(), overlay);
    if (it != m_Layers.end())
    {
        overlay->OnDetach();
        m_Layers.erase(it);
    }
}

// ========================================================================
// BATCH OPERATIONS
// ========================================================================

void LayerStack::UpdateAll(float dt)
{
    for (Layer* layer : m_Layers)
        layer->OnUpdate(dt);
}

void LayerStack::RenderUIAll()
{
    for (Layer* layer : m_Layers)
        layer->OnUIRender();
}

void LayerStack::Clear()
{
    for (auto it = m_Layers.rbegin(); it != m_Layers.rend(); ++it)
    {
        (*it)->OnDetach();
        delete *it;
    }
    m_Layers.clear();
    m_LayerInsertIndex = 0;
}

} // namespace RS
