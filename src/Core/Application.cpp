#include "../pch.hpp"

#include "Raysim/Core/Application.hpp"

namespace RS {

//==============================================================================
// Constructor
//==============================================================================

Application::Application(const ApplicationConfig& config)
    : backendType_(BackendType::Raylib), config_(config)
{
}

//==============================================================================
// Lifecycle
//==============================================================================

void Application::Run()
{
    try
    {
        if (!Logger::IsInitialized())
        {
            Logger::Init("RaySim", config_.logFile, true);
            Logger::SetConsoleLevel(spdlog::level::info);
            Logger::SetFileLevel(spdlog::level::trace);
        }

        RunInternal();
    }
    catch (const std::exception& e)
    {
        Shutdown();
        LOG_ERROR("Unhandled exception: {}", e.what());
    }
    catch (...)
    {
        Shutdown();
        LOG_ERROR("Unhandled unknown exception in main loop");
    }
}

void Application::RunInternal()
{
    LOG_INFO("[SESSION START] Initialising backend: {}", static_cast<int>(backendType_));

    // Create backend objects via factory
    Renderer = BackendFactory::CreateRenderer(backendType_);
    Window   = BackendFactory::CreateWindow(backendType_);
    Input    = BackendFactory::CreateInput(backendType_);

    Window->Init(WindowProps{config_.title, config_.width, config_.height});
    Time::Reset(); // Ensure time starts at zero when the main loop begins

    LOG_INFO("Backend ready. Calling Setup()...");
    Setup();
    LOG_INFO("Setup() done, entering main loop");

    isRunning_ = true;

    while (isRunning_ && !Window->ShouldClose())
    {
        Time::BeginFrame();

        Input->Update();

        // Fixed timestep updates
        int stepsTaken = 0;
        while (Time::ShouldFixedStep() && stepsTaken < config_.maxFixedSteps)
        {
            FixedUpdate(Time::GetFixedDeltaTime());
            ++stepsTaken;
        }

        if (stepsTaken == config_.maxFixedSteps)
        {
            LOG_WARN("Frame drop detected! Fixed steps clamped to {}", config_.maxFixedSteps);
        }

        Update(Time::GetDeltaTime()); // frame-based logic

        Renderer->BeginFrame();
        Draw(Time::GetInterpolationAlpha());
        Renderer->EndFrame();

        if (Time::GetFrameCount() % 300 == 0)
            Logger::Flush();

        Time::EndFrame();
    }

    Shutdown();
}

void Application::Shutdown()
{
    LOG_INFO("Shutting down application");
    Logger::Flush();

    Renderer.reset();
    Input.reset();
    Time::Reset();
    Window.reset(); // Destructor closes the OS window

    isRunning_ = false;
}

//==============================================================================
// Facade - window
//==============================================================================

void Application::SetTitle(const std::string& title)
{
    config_.title = title;
    if (Window) Window->SetWindowTitle(title);
}

void Application::SetSize(int width, int height)
{
    config_.width  = static_cast<uint32_t>(width);
    config_.height = static_cast<uint32_t>(height);
    if (Window) Window->SetWindowSize(width, height);
}

void Application::ToggleFullscreen()
{
    if (Window) Window->ToggleFullscreen();
}

bool Application::IsFullscreen() const
{
    return Window ? Window->IsFullscreen() : false;
}

int Application::GetWidth() const
{
    return Window ? Window->GetWidth() : static_cast<int>(config_.width);
}

int Application::GetHeight() const
{
    return Window ? Window->GetHeight() : static_cast<int>(config_.height);
}

float Application::GetAspectRatio() const
{
    return Window ? Window->GetAspectRatio() : 1.0f;
}

Vector2 Application::GetWindowSize() const
{
    return Window ? Window->GetWindowSize()
                   : Vector2{static_cast<float>(config_.width),
                              static_cast<float>(config_.height)};
}

//==============================================================================
// Facade - rendering
//==============================================================================

void Application::Background(const Color& color)
{
    Renderer->ClearScreen(color);
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
    Random::Seed(seed);
}

} // namespace RS
