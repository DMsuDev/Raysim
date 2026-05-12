#include "rspch.hpp"
#include "Raysim/Core/Application.hpp"

// --- Properties and enums ---
#include "Raysim/Graphics/Color.hpp"
#include "Raysim/Input/KeyCodes.hpp"

// --- Events ---
#include "Raysim/Events/Events.hpp"

// --- Input ---
#include "Raysim/Input/Input.hpp"

// --- Core system headers ---
#include "Raysim/Renderer/RenderCommand.hpp"
#include "Raysim/Input/Input.hpp"
#include "Raysim/Core/Window.hpp"
#include "Raysim/Core/Time.hpp"
#include "Raysim/Fonts/FontManager.hpp"
#include "Raysim/Fonts/Providers/STBTrueTypeProvider.hpp"

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

    try
    {
        // Create backends
        m_Window = BackendFactory::CreateAppWindow(m_Configuration.Window);
        m_Input = BackendFactory::CreateInput();
        m_EventBus = CreateScope<EventBus>();

        RS_CORE_ASSERT(m_Window, "Failed to create window backend");
        RS_CORE_ASSERT(m_Input, "Failed to create input backend");
        RS_CORE_ASSERT(m_EventBus, "Failed to create event bus backend");

        // Initialise RenderCommand backend
        auto api = BackendFactory::CreateRenderer();
        RS_CORE_ASSERT(api, "Failed to create renderer backend");

        RS::RenderCommand::Init(std::move(api));

        // -- Event callback ----------------------------------------------------
        m_Window->SetEventCallback([this](Event& e) { OnEvent(e); });

        // -- Font system --------------------------------------------------------
        FontManager::SetProvider(CreateScope<STBTrueTypeProvider>());
        FontManager::SetRenderer(BackendFactory::CreateFontRenderer());

        // -- Populate EngineContext ---------------------------------------------
        RebuildContext();

        // -- SceneManager ------------------------------------------------------
        m_SceneManager = CreateScope<SceneManager>(m_EngineContext);
        RS_CORE_ASSERT(m_SceneManager, "Failed to create scene manager backend");
        m_EngineContext.Scenes = m_SceneManager.get();
    }
    catch (const std::exception& e)
    {
        RS_CORE_CRITICAL("Application initialization failed: {}", e.what());
        throw; // Rethrow to be caught by the entry point
    }
    catch (...)
    {
        RS_CORE_CRITICAL("Application initialization failed due to an unknown error.");
        throw; // Rethrow to be caught by the entry point
    }

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
    m_EngineContext.Bus         = m_EventBus.get();
}

// ============================================================================
// Event handling
// ============================================================================

void Application::OnEvent(Event& e)
{
    RS_PROFILE_FUNCTION();

    EventDispatcher dispatcher(e);

    // Engine handles close and resize at the application level first.
    dispatcher.Dispatch<WindowCloseEvent>([this](WindowCloseEvent&)
    {
        RS_CORE_DEBUG("WindowCloseEvent received, shutting down.");
        m_Running = false;
        return true; // consumed
    });

    dispatcher.Dispatch<WindowResizeEvent>([this](WindowResizeEvent& re)
    {
        m_Minimized = (re.GetWidth() == 0 || re.GetHeight() == 0);
        return false; // let scene observe it too
    });

    // Forward to active scene (scene -> its layer stack in reverse order).
    if (!e.Handled && m_SceneManager)
        m_SceneManager->OnEvent(e);

    // Feed event-driven input backends (polling backends no-op this).
    if (m_Input)
        m_Input->OnEvent(e);

    // Broadcast to all bus subscribers (not stoppable by Handled).
    if (m_EventBus)
        m_EventBus->Publish(e);
}

// ============================================================================
// Main loop
// ============================================================================

void Application::Run()
{
    RS_PROFILE_FUNCTION();

    try
    {

    bool wasPausedDueToMinimize = false;

    auto& sm = *m_SceneManager; // Cache reference to avoid multiples pointer dereferences

    while (m_Running && !m_Window->ShouldClose())
    {
        RS_PROFILE_SCOPE("RunLoop");

        // Flush per-frame transient input state (Pressed, Released, Repeating)
        // before the window populates fresh events for this frame.
        {
            RS_PROFILE_SCOPE("InputUpdate");
            m_Input->Update();
        }

        // Poll window events (resize, close, etc.) and feed them into the event system.
        {
            RS_PROFILE_SCOPE("PollEvents");
            m_Window->PollEvents();
        }

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
                RS_PROFILE_SCOPE("FixedUpdate");
                sm.FixedUpdate(Time::GetFixedDeltaTime());
                ++stepsTaken;
            }

            if (stepsTaken == m_MaxFixedSteps)
                RS_CORE_WARN("Frame drop detected! Fixed steps clamped to {}", m_MaxFixedSteps);

            // Variable timestep update (delta time scaled by timeScale)
            {
                RS_PROFILE_SCOPE("Update");
                sm.Update(Time::GetDeltaTime());
            }
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
            RS_PROFILE_SCOPE("SwapBuffers");
            m_Window->SwapBuffers();
        }

        Time::EndFrame();

        if (m_Minimized)
        {
            // Sleep briefly to avoid busy-waiting while minimized to reduce CPU usage.
            std::this_thread::sleep_for(std::chrono::milliseconds(16));
        }
    }

    } // try
    catch (const std::exception& e)
    {
        RS_CORE_CRITICAL("Unhandled exception in main loop: {}", e.what());
        Close();
        throw;
    }
    catch (...)
    {
        RS_CORE_CRITICAL("Unhandled unknown exception in main loop.");
        Close();
        throw;
    }
}

void Application::Close()
{
    RS_PROFILE_FUNCTION();

    m_Running = false;

    RS_CORE_INFO("Shutting down application");
    Log::Flush();

    try
    {
        m_SceneManager.reset();
        m_Input.reset();
        m_Window.reset();

        Fonts::FontManager::Shutdown();
        RenderCommand::Shutdown();
    }
    catch (const std::exception& e)
    {
        RS_CORE_ERROR("Exception during shutdown: {}", e.what());
    }
    catch (...)
    {
        RS_CORE_ERROR("Unknown exception during shutdown.");
    }
}

} // namespace RS
