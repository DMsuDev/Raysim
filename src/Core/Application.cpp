#include "pch.hpp"

#include "Raysim/Core/Application.hpp"

namespace RS {

Application::Application(const ApplicationConfig& config)
    : m_Configuration(config)
{
    RS_PROFILE_FUNCTION();

    // --- Create backends ----------------------------------------------
    m_Window = BackendFactory::CreateAppWindow(m_Configuration.Backend, m_Configuration.Window);
    m_Input = BackendFactory::CreateInput(m_Configuration.Backend);

    RS_CORE_ASSERT(m_Window, "Failed to create window backend");
    RS_CORE_ASSERT(m_Input, "Failed to create input backend");

    // -- Initialise RenderCommand backend ------------------------------------
    auto api = BackendFactory::CreateRenderer(m_Configuration.Renderer);
    RS_CORE_ASSERT(api, "Failed to create renderer backend");

    RS::RenderCommand::Init(std::move(api));

    // -- Populate EngineContext ---------------------------------------------
    RebuildContext();

    // -- SceneManager ------------------------------------------------------
    m_SceneManager.emplace(m_EngineContext);

    RS_CORE_ASSERT(m_SceneManager, "Failed to create SceneManager");

    RS_CORE_INFO("Application '{}' ready.", m_Configuration.Window.Title);
}

Application::~Application() noexcept
{
    RS_PROFILE_FUNCTION();

    RS_CORE_INFO("Destroying application '{}'", m_Configuration.Window.Title);
}

// ============================================================================
// Context
// ============================================================================

void Application::RebuildContext()
{
    RS_PROFILE_FUNCTION();

    m_EngineContext.Config      = &m_Configuration;
    m_EngineContext.MainWindow  = m_Window.get();
    m_EngineContext.InputSystem = m_Input.get();
}

// ============================================================================
// Main loop
// ============================================================================

void Application::Run()
{
    RS_PROFILE_FUNCTION();

    bool wasPausedDueToMinimize = false;

    auto& sm = *m_SceneManager; // Cache reference to avoid multiples pointer dereferences

    while (m_Running && !m_Window->ShouldClose())
    {
        RS_PROFILE_SCOPE("RunLoop");

        m_Window->PollEvents();
        m_Minimized = m_Window->IsMinimized();

        if (m_Minimized)
        {
            if (!wasPausedDueToMinimize)
            {
                RS_CORE_DEBUG("Window minimized, pausing scene.");
                RS_PROFILE_SCOPE("Pause for Minimize");
                sm.Pause();
                wasPausedDueToMinimize = true;
            }
        }
        else
        {
            // If we were previously paused due to minimization, resume the scene now that we're no longer minimized
            if (wasPausedDueToMinimize)
            {
                RS_CORE_DEBUG("Window restored, resuming scene.");
                RS_PROFILE_SCOPE("Resume from Minimize");
                sm.Resume();
                wasPausedDueToMinimize = false;
            }
        }

        Time::BeginFrame();

        if (!m_Minimized && !Time::IsPaused())
        {
            RS_PROFILE_SCOPE("UpdateScene");

            // Fixed timestep updates
            uint32_t stepsTaken = 0;
            while (Time::ShouldFixedStep() && stepsTaken < m_MaxFixedSteps)
            {
                sm.FixedUpdate(Time::GetFixedDeltaTime());
                ++stepsTaken;
            }

            if (stepsTaken == m_MaxFixedSteps)
            {
                RS_CORE_WARN("Frame drop detected! Fixed steps clamped to {}", m_MaxFixedSteps);
            }

            sm.Update(Time::GetDeltaTime());
        }

        RS::RenderCommand::BeginFrame();

        if (!m_Minimized)
        {
            RS_PROFILE_SCOPE("Draw");
            sm.Draw(Time::GetInterpolationAlpha());
        }

        RS::RenderCommand::EndFrame();

        if (!m_Minimized)
        {
            m_Window->SwapBuffers();
        }

        Time::EndFrame();

        if (m_Minimized)
        {
            // Sleep briefly to avoid busy-waiting while minimized to reduce CPU usage.
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }
}

void Application::Close()
{
    RS_PROFILE_FUNCTION();

    m_Running = false;

    RS_CORE_INFO("Shutting down application");
    Log::Flush();

    m_SceneManager.reset();
    m_Input.reset();
    m_Window.reset();

    RS::RenderCommand::Shutdown();
}

//==============================================================================
// Facade - fonts / resources
//==============================================================================

void Application::SetDefaultFont(const std::string& fontPath, int fontSize)
{
    FontManager::LoadFont(fontPath, fontSize);
}

//==============================================================================
// Facade - misc
//==============================================================================

void Application::SetRandomSeed(unsigned int seed)
{
    Math::Random::Seed(seed);
}

} // namespace RS
