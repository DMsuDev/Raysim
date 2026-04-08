#pragma once

#include "Raysim/Core/EngineContext.hpp"

namespace RS {

class SceneManager; // Forward declaration to avoid circular dependency

/**
 * @class Scene
 * @brief Base class for all application scenes.
 *
 * Each Scene receives an EngineContext from the Application so it can
 * access the Window, Renderer, and Input subsystems without going
 * through a global singleton.
 */
class Scene
{
    friend class Application;

public:
    Scene() = default;

    /**
     * @brief Construct a named scene.
     * @param name  Display/debug name for the scene.
     * @param id    Unique numeric identifier (defaults to 0).
     */
    Scene(const std::string& name, uint16_t id = 0)
        : m_Name(name), m_SceneID(id) {}

    virtual ~Scene() = default;

// ===========================================================================
// Operators
// ===========================================================================

    Scene(const Scene&)            = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&)                 = default;
    Scene& operator=(Scene&&)      = default;

// ===========================================================================
// Lifecycle callbacks (override in derived scenes)
// ===========================================================================

    /// @brief Called once when the scene is first attached to the SceneManager.
    /// Use it for one-time setup: load assets, create layers, etc.
    virtual void OnAttach() {}

    /// @brief Called when the scene is removed or replaced.
    /// Clean up resources, detach layers, etc.
    virtual void OnDetach() {}

// ===========================================================================
// Scene control
// ===========================================================================

    /// @brief Check if the scene is currently running.
    /// @return True if the scene is running, false otherwise.
    bool IsRunning() const { return m_IsRunning; }

    /// @brief Check if the scene is currently paused.
    /// @return True if the scene is paused, false otherwise.
    bool IsPaused() const { return m_IsPaused; }

    /// @brief Pause or resume the scene.
    /// @param paused True to pause the scene, false to resume.
    void SetPaused(bool paused) { m_IsPaused = paused; }

    /// @brief Get the unique identifier for this scene.
    /// @return The scene's unique identifier.
    uint16_t GetSceneID() const { return m_SceneID; }

    /// @brief Get the name of the scene.
    /// @return The scene's name.
    const std::string& GetName() const { return m_Name; }

protected:
// ===========================================================================
// Lifecycle methods
// ===========================================================================

    /// @brief Called each time the scene is started.
    /// Use it to initialize resources, set up the scene, etc.
    virtual void OnStart() { }

    /// @brief Called every frame for logic updates.
    /// @param dt Time elapsed since the last frame (in seconds).
    virtual void OnUpdate(float dt) { (void)dt; }

    /// @brief Called at a fixed timestep for physics or deterministic updates.
    /// @param fixedDt Time elapsed since the last fixed update (in seconds).
    virtual void OnFixedUpdate(float fixedDt) { (void)fixedDt; }

    /// @brief Called to render the scene.
    /// Is called every frame after Update() and FixedUpdate().
    /// Call draw commands here. Avoid mutating game state inside Draw().
    /// @param alpha Interpolation factor in [0, 1) for blending physics snapshots. Same as Time::GetInterpolationAlpha().
    virtual void OnDraw(float alpha) { (void)alpha; }

    /// @brief Called when the scene is paused.
    /// Use it to pause animations, stop timers, etc.
    virtual void OnPause() { }

    /// @brief Called when the scene is resumed from a paused state.
    /// Use it to resume animations, restart timers, etc.
    virtual void OnResume() { }

// ===========================================================================
// Engine context access
// ===========================================================================

    /// @brief Get the engine context bound to this scene.
    /// @return The engine context.
    const EngineContext& GetContext() const { return m_Context; }

private:
    void Setup(EngineContext& ctx);  // Called once before the main loop starts
    void Run();                      // Dispatches lifecycle callbacks in the correct order

    friend class SceneManager;    // Allow SceneManager to call private Run/Shutdown methods
    friend class Application;     // Allow Application to bind EngineContext before pushing

private:
    EngineContext m_Context;      // Engine subsystems available to this scene

    std::string   m_Name;         // Name of the scene (for lookup and debugging)
    uint16_t      m_SceneID;      // Unique identifier for this scene

    bool m_Initialized = false;   // Whether the scene has been attached to the manager at least once (used to call OnAttach)
    bool m_IsRunning = false;     // Current running state of the scene
    bool m_IsPaused = false;      // Current paused state of the scene
};

} // namespace RS
