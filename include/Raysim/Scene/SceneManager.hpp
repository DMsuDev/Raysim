#pragma once

#include "Raysim/Core/Memory.hpp"
#include "Raysim/Core/EngineContext.hpp"
#include "Raysim/Scene/Scene.hpp"
#include <vector>

namespace RS {

/**
 * @class SceneManager
 * @brief Manages a LIFO stack of Scenes.
 *
 * Scenes are stored as `Scope<Scene>` (unique_ptr). Only the **top scene** is active
 * and receives runtime calls; scenes below are paused but remain in memory.
 *
 * Main operations:
 * - **AddScene(scene):** Pauses the current top scene and pushes a new one.
 * - **RemoveScene():** Removes the top scene and resumes the one below (if any).
 * - **SetScene(scene):** Clears the stack and sets a single active scene.
 *
 * Lookup helpers (`GetSceneByID`, `GetSceneByName`) perform linear searches for infrequent use.
 *
 * Helpers:
 * - `GetCurrentScene()`: Returns the top (active) scene.
 * - `GetUnderlyingScene()`: Returns the scene below the top, or nullptr if none.
 *
 * This stack-based design allows overlays (e.g., pause menus) without destroying underlying scenes.
 */
class SceneManager
{
public:
    SceneManager(EngineContext& ctx) : m_Context(ctx) {}
    ~SceneManager() = default;

// ===========================================================================
// Stack operations
// ===========================================================================

    /**
     * @brief Add a scene onto the stack.
     *
     * The currently active scene (if any) is paused but **not** detached.
     * The new scene becomes active and its Setup() method is called.
     *
     * @param scene Owning pointer to the scene. Must not be null.
     */
    void AddScene(Scope<Scene> scene);

    /**
     * @brief Remove the top scene off the stack.
     *
     * The top scene is detached (OnDetach) and destroyed.
     * If another scene remains on the stack it is resumed (unpaused).
     * Does nothing if the stack is empty.
     */
    void RemoveScene();

    /**
     * @brief Replace the entire stack with a single new scene.
     *
     * Every scene currently on the stack is detached and destroyed.
     * The new scene is pushed and its Setup() method is called.
     *
     * @param newScene Owning pointer to the scene. Must not be null.
     */
    void SetScene(Scope<Scene> newScene);

// ===========================================================================
// Flow control
// ===========================================================================

    /**
     * @brief Pause the currently active scene.
     *
     * The active scene's OnPause() method is called.
     * Does nothing if the stack is empty or the scene is already paused.
     */
    void PauseCurrentScene();

    /**
     * @brief Resume the currently active scene.
     *
     * The active scene's OnResume() method is called.
     * Does nothing if the stack is empty or the scene is not paused.
     */
    void ResumeCurrentScene();

// ===========================================================================
// Getters
// ===========================================================================

    /// @brief Returns the active (top) scene, or nullptr if the stack is empty.
    Scene& GetCurrentScene() const;

    /// @brief Linear search by scene ID. Returns nullptr if not found.
    Scene* GetSceneByID(uint16_t sceneID) const;

    /// @brief Linear search by scene name. Returns nullptr if not found.
    Scene* GetSceneByName(const std::string& name) const;

    /// @brief Returns the scene below the active one, or nullptr if there is none.
    Scene* GetUnderlyingScene() const;

    /// @brief Returns the number of scenes on the stack.
    size_t GetSceneCount() const { return m_SceneStack.size(); }

    /// @brief Returns true if the stack contains no scenes.
    bool IsEmpty() const { return m_SceneStack.empty(); }

private:
    /// @brief Helper function to get the active (top) scene.
    /// @return Pointer to the active scene, or nullptr if none.
    Scene* TryGetCurrentScene() const;

    /// @brief Internal method to set the pause state of the current scene.
    /// @param pause True to pause, false to resume.
    void SetPauseState(bool pause);

    /// @brief Internal method to clean up a scene before removal.
    /// @param scene Reference to the scene to clean up (calls OnDetach and stops it).
    void CleanupScene(Scene& scene);

    /// @brief Push a new scene onto the stack.
    /// @param scene Owning pointer to the scene. Must not be null.
    /// @return Reference to the newly pushed scene.
    Scene& PushScene(Scope<Scene> scene);

private:
    std::vector<Scope<Scene>> m_SceneStack;
    EngineContext& m_Context; // Reference to the engine context for scene setup
};

} // namespace RS
