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
    m_EngineContext.Config    = &m_Configuration;
    m_EngineContext.Renderer  = m_RenderCommand.get();
    m_EngineContext.Window    = m_Window.get();
    m_EngineContext.Input     = m_Input.get();
}

// ============================================================================
// Main loop
// ============================================================================

void Application::Run()
{
    Time::Reset(); // Ensure time starts at zero when the main loop begins

    RS_CORE_INFO("Backend ready. Calling OnStart()...");
    OnStart();
    RS_CORE_INFO("OnStart() done, entering main loop");

    m_Running = true;

    while (m_Running && !m_Window->ShouldClose())
    {
        Time::BeginFrame();

        m_Input->Update();


        // Fixed timestep updates
        uint32_t stepsTaken = 0;
        while (Time::ShouldFixedStep() && stepsTaken < m_Configuration.MaxFixedSteps)
        {
            OnFixedUpdate(Time::GetFixedDeltaTime());
            ++stepsTaken;
        }

        if (stepsTaken == m_Configuration.MaxFixedSteps)
        {
            RS_CORE_WARN("Frame drop detected! Fixed steps clamped to {}", m_Configuration.MaxFixedSteps);
        }

        OnUpdate(Time::GetDeltaTime()); // frame-based logic

        m_RenderCommand->BeginFrame();
        OnDraw(Time::GetInterpolationAlpha());
        m_RenderCommand->EndFrame();

        if (Time::GetFrameCount() % 300 == 0)
            Log::Flush();

        Time::EndFrame();
    }

    Close();
}

void Application::Close()
{
    RS_CORE_INFO("Shutting down application");
    Log::Flush();

    m_RenderCommand.reset();
    m_Renderer.reset();
    m_Input.reset();
    m_Window.reset(); // Destructor closes the OS window

    Time::Reset();

    m_Running = false;
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
