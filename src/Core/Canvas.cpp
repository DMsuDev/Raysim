#include "Raysim/Raysim.hpp"

#include <raylib.h>

// Static current canvas pointer definition
RS::Canvas* RS::Canvas::currentCanvas_ = nullptr;

// Static accessor implementation
RS::Canvas* RS::Canvas::GetCurrentCanvas()
{
    return currentCanvas_;
}

void RS::Canvas::SetSize(int width, int height)
{
    LOG_INFO("Setting size to {}x{}", width, height);
    width_ = width;
    height_ = height;

    // If the window is already created, update the size
    if (::IsWindowReady()) {
        ::SetWindowSize(width_, height_);
        LOG_DEBUG("Window size updated");
    }
}

void RS::Canvas::SetTargetFPS(int fps)
{
    LOG_INFO("Setting target FPS to {}", fps);
    fps_ = fps;
    // Delegate to TimeManager to handle FPS control
    time_.SetTargetFPS(fps_);
    LOG_DEBUG("FPS target updated");
}

void RS::Canvas::ToggleFullscreen()
{
    if (::IsWindowFullscreen()) {
        // If currently fullscreen, switch to windowed mode with the last known size
        ::ToggleFullscreen();
        SetSize(width_, height_);
    } else {
        // If currently windowed, switch to fullscreen mode using the monitor's resolution
        SetSize(::GetMonitorWidth(0), ::GetMonitorHeight(0));
        ::ToggleFullscreen();
    }
}

void RS::Canvas::SetTitle(const std::string& title)
{
    LOG_INFO("Setting window title to '{}'", title);
    title_ = title;
    // If the window is already created, update the title
    if (::IsWindowReady()) {
        ::SetWindowTitle(title_.c_str());
        LOG_DEBUG("Window title updated");
    }
}

void RS::Canvas::SetDefaultFont(const std::string& fontPath, int fontSize)
{
    font_.LoadFont(fontPath, fontSize);
}

void RS::Canvas::Background(const RS::Color& color) {
    ::ClearBackground(::Color{color.r, color.g, color.b, color.a});
}

void RS::Canvas::RunInternal()
{
    LOG_INFO("[SESSION START] Starting application loop");

    // Initialize raylib window and OpenGL context
    ::InitWindow(width_, height_, title_.c_str());
    LOG_DEBUG("Window initialized with size {}x{}", width_, height_);

    // Set target FPS for the main loop
    SetTargetFPS(fps_);

    Setup();
    LOG_INFO("Setup completed, entering main loop");

    isRunning_ = true;

    int frameCount = 0;
    while (isRunning_ && !::WindowShouldClose())
    {
        frameCount++;

        // Update input and time managers
        input_.Update();
        time_.Update();

        Update();

        ::BeginDrawing();

        Draw();

        ::EndDrawing();

        // Each 300 frames, execute the logic if needed (e.g., flush logs, perform maintenance tasks)
        if (frameCount % 300 == 0) { RS::Logger::Flush(); }
    }

    Shutdown();
}

void RS::Canvas::Shutdown()
{
    LOG_INFO("Shutting down application");
    if (::IsWindowReady()) {
        ::CloseWindow();
        LOG_DEBUG("Window closed successfully");
    }
    // Final flush to ensure all logs are written before exit
    Logger::Flush();

    currentCanvas_ = nullptr;
    isRunning_ = false;
}

void RS::Canvas::Run()
{
    if (currentCanvas_ != nullptr)
    {
        LOG_ERROR("A Canvas is already running. Multiple instances are not supported.");
        return;
    }
    currentCanvas_ = this;

    try {
        // Only log errors to avoid cluttering the console
        ::SetTraceLogLevel(LOG_ERROR);

        if (!Logger::IsInitialized()) {
            Logger::Init("RaySim", "App.log", true);
            Logger::SetConsoleLevel(spdlog::level::info);
            Logger::SetFileLevel(spdlog::level::trace);
        }

        RunInternal();
    } catch (const std::exception& e) {
        Shutdown();
        LOG_ERROR("Unhandled exception in main loop: {}", e.what());
    } catch (...) {
        Shutdown();
        LOG_ERROR("Unhandled unknown exception in main loop");
    }
}
