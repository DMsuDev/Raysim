/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 *   Raysim is licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

#pragma once

#include "Raysim/Core/Base.hpp"

#include "Raysim/Core/EngineContext.hpp"
#include "Raysim/Core/LayerStack.hpp"
#include "Raysim/Renderer/RenderCommand.hpp"
#include "Raysim/Events/Event.hpp"

namespace RS {

class SceneManager; // Forward declaration
class ImGuiLayer;   // Forward declaration
class ImGuiBackend; // Forward declaration

// ============================================================
// Type ID system
// ===========================================================

using SceneID = uint32_t;

inline SceneID GenerateSceneID() {
    static SceneID counter = 1;
    return counter++;
}

template<typename T>
SceneID GetSceneTypeID() {
    static SceneID id = GenerateSceneID();
    return id;
}

// ============================================================
// MACRO for defining scene types
// ===========================================================================

#define RS_SCENE(type) \
public: \
    static RS::SceneID StaticID() noexcept { return RS::GetSceneTypeID<type>(); } \
    static const char* StaticName() noexcept { return #type; } \
    type(RS::EngineContext* ctx) : Scene(ctx) {} \

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
private:
    // Key pattern: Only the SceneManager can create and manage scenes.
    // The Key struct is an opaque token that prevents external code from calling the protected constructor.
    struct Key { friend class SceneManager; private: Key() {} };

public:
    virtual ~Scene() = default;
    Scene(EngineContext* context) : m_Context(context) {}

// ===========================================================================
// Operators
// ===========================================================================

    Scene(const Scene&)            = delete;
    Scene& operator=(const Scene&) = delete;
    Scene(Scene&&)                 = default;
    Scene& operator=(Scene&&)      = default;

// ===========================================================================
// Non-virtual public interface (NVI)
// ===========================================================================

    void Start(Key)
    {
        RS_PROFILE_FUNCTION();
        if (m_State == SceneState::Uninitialized)
        {
            RS_CORE_DEBUG("Scene '{}': starting", m_Config.Name);
            OnStart();
            m_State = SceneState::Running;
        }
    }

    void Update([[maybe_unused]] float dt, Key)
    {
        if (m_State == SceneState::Running)
        {
            {
                RS_PROFILE_SCOPE("Scene::OnUpdate");
                OnUpdate(dt);
            }

            {
                RS_PROFILE_SCOPE("Scene::LayerUpdate");
                m_LayerStack.UpdateAll(dt);
            }
        }
    }

    void FixedUpdate([[maybe_unused]] float dt, Key)
    {
        if (m_State == SceneState::Running)
            OnFixedUpdate(dt);
    }

    void Draw([[maybe_unused]] float alpha, Key)
    {
        // Can draw even if paused, but not if uninitialized or stopped
        if (m_State != SceneState::Uninitialized)
        {
            {
                RS_PROFILE_SCOPE("Scene::OnDraw");
                OnDraw(alpha);
            }

            {
                RS_PROFILE_SCOPE("Scene::ImGui");
                ImGuiBeginFrame();
                m_LayerStack.RenderUIAll();
                ImGuiEndFrame();
            }
        }
    }

    void Pause(Key)
    {
        RS_PROFILE_FUNCTION();
        if (m_State == SceneState::Running)
        {
            RS_CORE_DEBUG("Scene '{}': paused", m_Config.Name);
            m_State = SceneState::Paused;
            OnPause();
        }
    }

    void Resume(Key)
    {
        RS_PROFILE_FUNCTION();
        if (m_State == SceneState::Paused)
        {
            RS_CORE_DEBUG("Scene '{}': resumed", m_Config.Name);
            m_State = SceneState::Running;
            OnResume();
        }
    }

    void Stop(Key)
    {
        RS_PROFILE_FUNCTION();
        if (m_State != SceneState::Uninitialized)
        {
            RS_CORE_DEBUG("Scene '{}': stopping", m_Config.Name);
            m_LayerStack.Clear();
            m_ImGuiLayer = nullptr;
            OnDetach();
            m_State = SceneState::Uninitialized;
        }
    }

    /// @brief Dispatch an event to this scene.
    ///
    /// First calls the user-overridable OnEvent(), then propagates to the
    /// LayerStack in reverse order. Stops early if any handler sets e.Handled.
    void OnEvent(Event& e, Key)
    {
        OnEvent(e);
        if (!e.Handled)
            m_LayerStack.DispatchEvent(e);
    }

// ===========================================================================
// Public Information Accessors
// ===========================================================================

    bool IsRunning() const noexcept { return m_State == SceneState::Running; }
    bool IsPaused() const noexcept { return m_State == SceneState::Paused; }

    /// @brief Get the unique identifier for this scene.
    SceneID GetSceneID() const noexcept { return m_Config.ID; }
    const std::string& GetName() const noexcept { return m_Config.Name; }

protected:

// ===========================================================================
// Lifecycle methods
// ===========================================================================

    /// @brief Called once when the scene is first attached to the SceneManager.
    /// Use it for one-time setup: load assets, create layers, etc.
    virtual void OnAttach() {}

    /// @brief Called when the scene is removed or replaced.
    /// Clean up resources, detach layers, etc.
    virtual void OnDetach() {}

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

    /// @brief Called when an event is dispatched to this scene.
    ///
    /// Override to intercept events before they reach the LayerStack. Return
    /// without setting e.Handled to let layers below also process the event.
    /// @param e  The event. Use EventDispatcher for type-safe handling.
    virtual void OnEvent([[maybe_unused]] Event& e) {}

// ===========================================================================
// Layer management
// ===========================================================================

    /// @brief Push a layer onto the scene's layer stack. Calls OnAttach() immediately.
    void PushLayer(Layer* layer) { m_LayerStack.PushLayer(layer); }

    /// @brief Push an overlay onto the scene's layer stack. Overlays render on top.
    void PushOverlay(Layer* overlay) { m_LayerStack.PushOverlay(overlay); }

    /// @brief Remove a layer from the scene's layer stack. Calls OnDetach().
    void PopLayer(Layer* layer) { m_LayerStack.PopLayer(layer); }

    /// @brief Remove an overlay from the scene's layer stack. Calls OnDetach().
    void PopOverlay(Layer* overlay) { m_LayerStack.PopOverlay(overlay); }

    /// @brief Create and push an ImGuiLayer overlay using the application's configured backend.
    /// Returns a non-owning pointer to the created layer (owned by the LayerStack).
    ImGuiLayer* SetupImGuiLayer();

// ============================================================================
// Direct access to subsystems
// ============================================================================

    /// @brief Get the input subsystem from the engine context.
    inline Input& GetInput() noexcept { return *m_Context->InputSystem; }
    /// @brief Get the window subsystem from the engine context.
    inline Window& GetWindow() noexcept { return *m_Context->MainWindow; }
    /// @brief Get the SceneManager to trigger scene transitions from within a scene.
    /// @note Use this to call ChangeScene<T>(), ChangeScene(id), or ChangeScene(name).
    inline SceneManager& GetSceneManager() noexcept { return *m_Context->Scenes; }
    /// @brief Returns a proxy reference to the static RenderCommand dispatcher.
    /// @note RenderCommand is a purely static API, it holds no instance state.
    ///       This accessor exists only for API consistency and backward compatibility,
    ///       so scenes can write GetRenderer().ClearScreen(...) instead of
    ///       RS::RenderCommand::ClearScreen(...) directly.
    RenderCommand& GetRenderer() noexcept;

private:

    // Called internally by the NVI Draw() to wrap layer UI rendering with ImGui frame.
    void ImGuiBeginFrame();
    void ImGuiEndFrame();

    struct SceneConfig
    {
        // Name of the scene (for lookup and debugging)
        std::string Name = "UnnamedScene";
        SceneID ID = 0;
    };
    enum class SceneState
    {
        Uninitialized,
        Running,
        Paused,
    };

    EngineContext* m_Context    = nullptr;
    SceneState     m_State      = SceneState::Uninitialized;
    SceneConfig    m_Config;
    LayerStack     m_LayerStack;
    ImGuiLayer*    m_ImGuiLayer = nullptr; ///< Non-owning; owned by m_LayerStack

    friend class SceneManager;
};

} // namespace RS
