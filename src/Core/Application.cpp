#include "rspch.hpp"
#include "Raysim/Core/Application.hpp"

// --- Properties and enums ---
#include "Raysim/Graphics/Color.hpp"
#include "Raysim/Input/KeyCodes.hpp"

// --- Core system headers ---
#include "Raysim/Renderer/RenderCommand.hpp"
#include "Raysim/Input/Input.hpp"
#include "Raysim/Core/Window.hpp"
#include "Raysim/Core/Time.hpp"
#include "Raysim/Fonts/FontManager.hpp"
#include "Raysim/Fonts/Providers/STBTrueTypeProvider.hpp"

// --- Math ---
#include "Raysim/Math/Random.hpp"

// --- Backend factory ---
#include "Raysim/Core/BackendFactory.hpp"

namespace RS {
using namespace RS::Fonts;

Application::Application(const ApplicationConfig& config)
    : m_Configuration(config)
{
    RS_PROFILE_FUNCTION();

    // Apply the console log level from config (overrides the default set in Log::Init)
    Log::SetConsoleLevel(config.ConsoleLogLevel);

    // Create backends
    m_Window = BackendFactory::CreateAppWindow(m_Configuration.Window);
    m_Input = BackendFactory::CreateInput();

    RS_CORE_ASSERT(m_Window, "Failed to create window backend");
    RS_CORE_ASSERT(m_Input, "Failed to create input backend");

    // Initialise RenderCommand backend
    auto api = BackendFactory::CreateRenderer();
    RS_CORE_ASSERT(api, "Failed to create renderer backend");

    RS::RenderCommand::Init(std::move(api));

    // -- Font system --------------------------------------------------------
    FontManager::SetProvider(CreateScope<STBTrueTypeProvider>());
    FontManager::SetRenderer(BackendFactory::CreateFontRenderer());
    FontManager::LoadDefaultFont();

    // -- Populate EngineContext ---------------------------------------------
    RebuildContext();

    // -- SceneManager ------------------------------------------------------
    m_SceneManager = CreateScope<SceneManager>(m_EngineContext);
    m_EngineContext.Scenes = m_SceneManager.get();

    RS_CORE_INFO("Application '{}' ready.", m_Configuration.Window.Title);
}

Application::~Application() noexcept
{
    RS_PROFILE_FUNCTION();

    RS_CORE_INFO("Destroying application '{}'", m_Configuration.Window.Title);

    // Ensure all subsystems are released even if Close() was never called.
    // Close() is idempotent via m_Running - safe to call from the destructor.
    if (m_Running)
        Close();

    Log::Flush();
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

    Fonts::FontManager::Shutdown();
    RenderCommand::Shutdown();
}

//==============================================================================
// Facade - fonts / resources
//==============================================================================

void Application::SetDefaultFont(const std::string& fontPath, int fontSize)
{
    Fonts::FontManager::LoadFont("default", fontPath, fontSize);
}

//==============================================================================
// Facade - misc
//==============================================================================

void Application::SetRandomSeed(unsigned int seed)
{
    ::RS::Math::Random::Seed(seed);
}

} // namespace RS
