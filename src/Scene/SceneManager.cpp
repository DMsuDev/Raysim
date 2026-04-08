#include "../pch.hpp"

#include "Raysim/Scene/SceneManager.hpp"

namespace RS {

// ===================================================================
// Internal helpers
// ===================================================================

Scene* SceneManager::TryGetCurrentScene() const
{
    return m_SceneStack.empty() ? nullptr : m_SceneStack.back().get();
}

void SceneManager::SetPauseState(bool pause)
{
    if (auto* scene = TryGetCurrentScene())
    {
        if (scene->IsPaused() == pause)
        {
            RS_CORE_WARN("Scene '{}' (ID {}) already in state: {}",
                scene->GetName(),
                scene->GetSceneID(),
                pause ? "paused" : "running");
            return;
        }

        scene->SetPaused(pause);

        if (pause)
            scene->OnPause();
        else
            scene->OnResume();
    }
}

Scene& SceneManager::PushScene(Scope<Scene> scene)
{
    RS_CORE_ASSERT_TRUE(scene, "Cannot push null scene");

    m_SceneStack.push_back(std::move(scene));
    return *m_SceneStack.back();
}

// ===================================================================
// Getters
// ===================================================================

Scene& SceneManager::GetCurrentScene() const
{
    RS_CORE_ASSERT(!m_SceneStack.empty(), "No current scene available");
    return *m_SceneStack.back();
}

Scene* SceneManager::GetSceneByID(uint16_t sceneID) const
{
    if (m_SceneStack.empty())
    {
        RS_CORE_WARN("No scenes available to search for ID {}", sceneID);
        return nullptr;
    }

    for (const auto& scene : m_SceneStack)
    {
        if (scene->GetSceneID() == sceneID)
        {
            RS_CORE_INFO("Found scene '{}' with ID {}", scene->GetName(), sceneID);
            return scene.get();
        }
    }
    RS_CORE_WARN("No scene found with ID {}", sceneID);
    return nullptr;
}

Scene* SceneManager::GetSceneByName(const std::string& name) const
{
    if (m_SceneStack.empty())
    {
        RS_CORE_WARN("No scenes available to search for name '{}'", name);
        return nullptr;
    }

    for (const auto& scene : m_SceneStack)
    {
        if (scene->GetName() == name)
        {
            RS_CORE_INFO("Found scene '{}' with ID {}", name, scene->GetSceneID());
            return scene.get();
        }
    }
    RS_CORE_WARN("No scene found with name '{}'", name);
    return nullptr;
}

Scene* SceneManager::GetUnderlyingScene() const
{
    if (m_SceneStack.size() <= 1)
        return nullptr;

    return m_SceneStack[m_SceneStack.size() - 2].get();
}

// ===================================================================
// Flow control
// ===================================================================

void SceneManager::PauseCurrentScene()
{
    SetPauseState(true);
}

void SceneManager::ResumeCurrentScene()
{
    SetPauseState(false);
}

// ===================================================================
// Stack operations
// ===================================================================

void SceneManager::AddScene(Scope<Scene> scene)
{
    RS_CORE_ASSERT_TRUE(scene, "Cannot add a null scene to the SceneManager");

    PauseCurrentScene();

    Scene& sceneRef = PushScene(std::move(scene));
    sceneRef.Setup(m_Context);
}

void SceneManager::RemoveScene()
{
    if (m_SceneStack.empty())
        return;

    auto& current = GetCurrentScene();
    RS_CORE_INFO("Popping scene '{}' (ID {})", current.GetName(), current.GetSceneID());

    current.OnDetach();
    m_SceneStack.pop_back();

    ResumeCurrentScene();
}

void SceneManager::SetScene(Scope<Scene> newScene)
{
    RS_CORE_ASSERT_TRUE(newScene, "Cannot set a null scene in the SceneManager");

    // Detach all existing scenes
    while (!m_SceneStack.empty())
    {
        Scene& current = GetCurrentScene();
        RS_CORE_INFO("Scene '{}' (ID {}) detached", current.GetName(), current.GetSceneID());

        current.OnDetach();
        m_SceneStack.pop_back();
    }

    Scene& sceneRef = PushScene(std::move(newScene));
    sceneRef.Setup(m_Context);
}

} // namespace RS
