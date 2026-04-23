#include "../pch.hpp"

#include "Raysim/Scene/SceneManager.hpp"

namespace RS {

// ============================================================
// INTERNAL
// ============================================================

void SceneManager::HandleTransition()
{
    if (!m_NextScene)
        return;

    if (m_Current)
    {
        m_Current->Stop(MakeKey());
        m_Current->OnDetach();
    }

    m_Current = std::move(m_NextScene);

    m_Current->OnAttach();
    m_Current->Start(MakeKey());
}

// ===================================================================
// Change scene by type
// ===================================================================

void SceneManager::ChangeScene(SceneID id)
{
    auto it = m_Registry.find(id);
    if (it == m_Registry.end())
        return;

    const auto& desc = it->second;

    m_NextScene = desc.Factory(m_Context);

    m_NextScene->m_Config.ID = desc.ID;
    m_NextScene->m_Config.Name = desc.Name;
}

void SceneManager::ChangeScene(const std::string& name)
{
    auto it = m_NameToID.find(name);
    if (it == m_NameToID.end())
        return;

    ChangeScene(it->second);
}

} // namespace RS
