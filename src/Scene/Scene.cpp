#include "../pch.hpp"

#include "Raysim/Core/Time.hpp"
#include "Raysim/Core/ApplicationConfig.hpp"

#include "Raysim/Scene/Scene.hpp"

#include "Raysim/Renderer/RenderCommand.hpp"

namespace RS {

//==============================================================================
// Lifecycle
//==============================================================================

void Scene::Setup(EngineContext& ctx)
{
    RS_CORE_INFO("Setting up scene '{}' (ID {})", m_Name, m_SceneID);

    Time::Reset(); // Ensure time starts at zero when the main loop begins
    m_Context = ctx; // Bind the engine context to this scene

    m_IsRunning = true;
    m_IsPaused = false;

    if (m_Initialized)
    {
        OnResume();
    }
    else
    {
        OnAttach();
        m_Initialized = true;
    }

    OnStart();
}

void Scene::Run()
{
    Time::BeginFrame();

    if (!m_IsPaused)
    {
        // Fixed timestep updates
        uint32_t stepsTaken = 0;
        while (Time::ShouldFixedStep() && stepsTaken < m_Context.Config->MaxFixedSteps)
        {
            OnFixedUpdate(Time::GetFixedDeltaTime());
            ++stepsTaken;
        }

        if (stepsTaken == m_Context.Config->MaxFixedSteps)
        {
            RS_CORE_WARN("Frame drop detected! Fixed steps clamped to {}", m_Context.Config->MaxFixedSteps);
        }

        const float dt = Time::GetDeltaTime();
        OnUpdate(dt);
    }

    m_Context.Renderer->BeginFrame();
    OnDraw(Time::GetInterpolationAlpha());
    m_Context.Renderer->EndFrame();

    Time::EndFrame();
}

} // namespace RS
