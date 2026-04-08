#include "../pch.hpp"

#include "Raysim/Core/Application.hpp"

namespace RS {

Application::Application(const ApplicationConfig& config)
    : m_Configuration(config)
{
    // --- Create backends ----------------------------------------------
    m_Window   = BackendFactory::CreateWindow(m_Configuration.Backend, m_Configuration.Window);
    m_Renderer = Shared<RendererAPI>(BackendFactory::CreateRenderer(m_Configuration.Renderer).release());
    m_Input    = BackendFactory::CreateInput(m_Configuration.Backend);

    RS_CORE_ASSERT(m_Window,   "Failed to create window backend");
    RS_CORE_ASSERT(m_Renderer, "Failed to create renderer backend");
    RS_CORE_ASSERT(m_Input,    "Failed to create input backend");

    // -- Initialise renderer backend ------------------------------------
    m_Renderer->Init();

    // -- RenderCommand --------------------------------------------------
    m_RenderCommand = CreateScope<RenderCommand>(m_Renderer);
    RS_CORE_ASSERT(m_RenderCommand, "Failed to create RenderCommand");

    // -- Populate EngineContext ---------------------------------------------
    RebuildContext();

    // -- SceneManager ------------------------------------------------------
    m_SceneManager.emplace(m_EngineContext); // Create the SceneManager in-place

    RS_CORE_ASSERT(m_SceneManager, "Failed to create SceneManager");

    RS_CORE_INFO("Application '{}' ready.", m_Configuration.Window.Title);
}

Application::~Application() noexcept
{
    RS_CORE_INFO("Destroying application '{}'", m_Configuration.Window.Title);
    m_RenderCommand->Shutdown();
}

// ============================================================================
// Context
// ============================================================================

void Application::RebuildContext()
{
    m_EngineContext.Config      = &m_Configuration;
    m_EngineContext.Renderer    = m_RenderCommand.get();
    m_EngineContext.MainWindow  = m_Window.get();
    m_EngineContext.InputSystem = m_Input.get();
}

// ============================================================================
// Scene management
// ============================================================================

void Application::AddScene(Scope<Scene> scene)
{
    if (m_SceneManager) {
        m_SceneManager->AddScene(std::move(scene));
    } else {
        RS_CORE_ERROR("Cannot add scene: SceneManager is not initialized");
    }
}

void Application::SetScene(Scope<Scene> newScene)
{
    if (m_SceneManager) {
        m_SceneManager->SetScene(std::move(newScene));
    } else {
        RS_CORE_ERROR("Cannot set scene: SceneManager is not initialized");
    }
}

// ============================================================================
// Main loop
// ============================================================================

void Application::Run()
{
    bool wasPausedDueToMinimize = false;

    auto& sm = *m_SceneManager; // Cache reference to avoid multiples pointer dereferences

    while (m_Running && !m_Window->ShouldClose())
    {
        // Poll input events and swap back buffer
        m_Window->OnUpdate();
        m_Minimized = m_Window->IsMinimized();

        if (!m_Minimized)
        {
            if (wasPausedDueToMinimize)
            {
                // Resume the scene if it was paused due to minimize
                sm.ResumeCurrentScene();
                wasPausedDueToMinimize = false;
            }

            // Run the active scene (timing, fixed update, variable update, draw)
            auto& scene = sm.GetCurrentScene();
            scene.Run();
        }
        else
        {
            if (!wasPausedDueToMinimize)
            {
                RS_CORE_DEBUG("Window minimized, pausing scene.");
                printf("Window minimized, pausing scene.\n");
                sm.PauseCurrentScene();
                wasPausedDueToMinimize = true;
            }

            // Skip updating and rendering to save CPU/GPU resources
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }
}

void Application::Close()
{
    m_Running = false;

    RS_CORE_INFO("Shutting down application");
    Log::Flush();

    m_RenderCommand.reset();
    m_SceneManager.reset();
    m_Renderer.reset();
    m_Input.reset();
    m_Window.reset();

    Time::Reset();
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
