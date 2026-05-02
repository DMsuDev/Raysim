#include "pch.hpp"

#include "Raysim/Scene/SceneManager.hpp"

namespace RS {

// ============================================================
// INTERNAL
// ============================================================

void SceneManager::HandleTransition()
{
    // If no scene was ever requested via ChangeScene, auto-start the first registered one.
    if (!m_Current && !m_NextScene)
    {
        if (!m_HasRequestedScene && m_HasFirstRegistered)
        {
            RS_CORE_WARN(
                "No initial scene was set via ChangeScene(). "
                "Auto-starting the first registered scene: '{}'.",
                m_Registry[m_FirstRegisteredID].Name);
            ChangeScene(m_FirstRegisteredID);
        }
        else
        {
            return;
        }
    }

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
    {
        RS_CORE_ERROR("ChangeScene: no scene with ID {} is registered. Call RegisterScene<T>() first.", id);
        return;
    }

    m_HasRequestedScene = true;

    const auto& desc = it->second;

    m_NextScene = desc.Factory(m_Context);

    m_NextScene->m_Config.ID = desc.ID;
    m_NextScene->m_Config.Name = desc.Name;
}

void SceneManager::ChangeScene(const std::string& name)
{
    auto it = m_NameToID.find(name);
    if (it == m_NameToID.end())
    {
        RS_CORE_ERROR("ChangeScene: no scene named '{}' is registered. Call RegisterScene<T>() first.", name);
        return;
    }

    m_HasRequestedScene = true;
    ChangeScene(it->second);
}

} // namespace RS
