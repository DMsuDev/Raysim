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
        {
            scene->Stop();
            scene->OnPause();
        }
        else
            scene->OnResume();
    }
}

Scene& SceneManager::PushScene(Scope<Scene> scene)
{
    RS_CORE_ASSERT_TRUE(scene);

    m_SceneStack.push_back(std::move(scene));
    return *m_SceneStack.back();
}

void SceneManager::CleanupScene(Scene& scene)
{
    RS_CORE_INFO("Cleaning up scene '{}' (ID {})", scene.GetName(), scene.GetSceneID());

    scene.Stop();
    scene.OnDetach();

    // Remove the scene from the stack (destroying it)
    m_SceneStack.pop_back();
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
    RS_CORE_ASSERT_TRUE(scene);

    if (scene->GetSceneID() == 0)
        RS_CORE_WARN("Scene '{}' has no valid ID. Did you forget RS_SCENE?", scene->GetName());

    RS_CORE_INFO("Pushing scene '{}' (ID {}) on top of stack", scene->GetName(), scene->GetSceneID());

    PauseCurrentScene();

    Scene& sceneRef = PushScene(std::move(scene));
    sceneRef.Setup(m_Context);
}

void SceneManager::RemoveScene()
{
    if (m_SceneStack.empty())
        return;

    CleanupScene(GetCurrentScene());
    ResumeCurrentScene();
}

void SceneManager::SetScene(Scope<Scene> newScene)
{
    RS_CORE_ASSERT_TRUE(newScene);

    if (newScene->GetSceneID() == 0)
        RS_CORE_WARN("Scene '{}' has no valid ID. Did you forget RS_SCENE?", newScene->GetName());

    // Detach all existing scenes
    while (!m_SceneStack.empty())
    {
        CleanupScene(GetCurrentScene());
    }

    Scene& sceneRef = PushScene(std::move(newScene));
    sceneRef.Setup(m_Context);
}

} // namespace RS
