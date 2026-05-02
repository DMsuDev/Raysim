#pragma once

#include "Raysim/Core/Base.hpp"

#include "Raysim/Scene/Scene.hpp"

#include <unordered_map>
#include <memory>
#include <functional>
#include <string>

namespace RS {

class SceneManager
{
public:
    explicit SceneManager(EngineContext& ctx)
        : m_Context(ctx) {}

// ============================================================
// REGISTRY
// ============================================================

    /// @brief Register a scene type so it can later be activated by ID or name.
    /// @tparam T  Scene type. Must use the RS_SCENE(T) macro.
    /// @note The first scene registered becomes the automatic fallback if
    ///       ChangeScene() is never called before the first frame.
    template<typename T>
    void RegisterScene()
    {
        SceneDescriptor desc;
        desc.ID = T::StaticID();
        desc.Name = T::StaticName();

        desc.Factory = [](EngineContext& ctx) {
            return CreateScope<T>(&ctx);
        };

        m_NameToID[desc.Name] = desc.ID;
        m_Registry[desc.ID] = std::move(desc);

        if (!m_HasFirstRegistered)
        {
            m_FirstRegisteredID = T::StaticID();
            m_HasFirstRegistered = true;
        }
    }

// ============================================================
// CHANGE SCENE
// ============================================================

    /// @brief Immediately queue a scene transition by type (compile-time overload).
    /// @tparam T     Scene type. Must use the RS_SCENE(T) macro.
    /// @tparam Args  Optional extra constructor arguments forwarded to T.
    /// @note This overload constructs the scene directly — it does NOT require
    ///       the scene to have been previously registered via RegisterScene<T>().
    ///       If you need to switch scenes by ID or name at runtime, register first
    ///       and use the ChangeScene(SceneID) / ChangeScene(string) overloads instead.
    template<typename T, typename... Args>
    void ChangeScene(Args&&... args)
    {
        if (m_Registry.find(T::StaticID()) == m_Registry.end())
        {
            RS_CORE_WARN(
                "ChangeScene<{}>: scene type was not registered via RegisterScene<{}>()."
                " It will still be activated, but ChangeScene(ID/name) won't find it.",
                T::StaticName(), T::StaticName());
        }

        m_HasRequestedScene = true;

        m_NextScene = CreateScope<T>(&m_Context, std::forward<Args>(args)...);

        m_NextScene->m_Config.ID = T::StaticID();
        m_NextScene->m_Config.Name = T::StaticName();
    }

    /// @brief Queue a scene transition by its registered SceneID.
    /// @note Requires the scene to have been registered via RegisterScene<T>() first.
    ///       Logs an error and does nothing if the ID is not found.
    void ChangeScene(SceneID id);

    /// @brief Queue a scene transition by its registered name.
    /// @note Requires the scene to have been registered via RegisterScene<T>() first.
    ///       Logs an error and does nothing if the name is not found.
    void ChangeScene(const std::string& name);

// ============================================================
// LOOP
// ============================================================

    void Update(float dt)
    {
        HandleTransition();

        if (m_Current)
            m_Current->Update(dt, MakeKey());
    }

    void FixedUpdate(float dt)
    {
        if (m_Current)
            m_Current->FixedUpdate(dt, MakeKey());
    }

    void Draw(float alpha)
    {
        if (m_Current)
            m_Current->Draw(alpha, MakeKey());
    }

// ============================================================
// CONTROL
// ============================================================

    void Pause()
    {
        if (m_Current)
            m_Current->Pause(MakeKey());
    }

    void Resume()
    {
        if (m_Current)
            m_Current->Resume(MakeKey());
    }

// ============================================================
// ACCESS
// ============================================================

    Scene* GetCurrentScene() const { return m_Current.get(); }

// ============================================================
// INTERNAL
// ============================================================

private:

    void HandleTransition();
    Scene::Key MakeKey() { return Scene::Key{}; }

private:

    using SceneFactory = std::function<Scope<Scene>(EngineContext&)>;

    struct SceneDescriptor
    {
        SceneID ID;
        std::string Name;
        SceneFactory Factory;
    };

private:
    EngineContext& m_Context;

    Scope<Scene> m_Current;
    Scope<Scene> m_NextScene;

    std::unordered_map<SceneID, SceneDescriptor> m_Registry;
    std::unordered_map<std::string, SceneID>  m_NameToID;

    SceneID m_FirstRegisteredID  = 0;
    bool    m_HasFirstRegistered = false;
    bool    m_HasRequestedScene  = false;
};

} // namespace RS
