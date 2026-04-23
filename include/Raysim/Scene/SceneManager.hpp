#pragma once

#include "Raysim/Core/Memory.hpp"
#include "Raysim/Core/EngineContext.hpp"
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
    }

// ============================================================
// CHANGE SCENE
// ============================================================

    template<typename T, typename... Args>
    void ChangeScene(Args&&... args)
    {
        m_NextScene = CreateScope<T>(&m_Context, std::forward<Args>(args)...);

        m_NextScene->m_Config.ID = T::StaticID();
        m_NextScene->m_Config.Name = T::StaticName();
    }

    void ChangeScene(SceneID id);
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
};

} // namespace RS
