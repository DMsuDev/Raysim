#include "Raysim/Raysim.hpp"

#include <raylib.h>

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

void RS::Canvas::Run()
{
    Logger::Init("RaySim", "App.log", true);  // Initialize logger before use
    Logger::SetConsoleLevel(spdlog::level::info); // Set log level to trace for detailed output

    LOG_INFO("[SESSION START] Startup begin");

    SetTraceLogLevel(LOG_ERROR); // Only log errors to avoid cluttering the console

    LOG_INFO("Starting canvas application");
    InitWindow(width_, height_, title_.c_str());

    SetTargetFPS(fps_);

    LOG_INFO("Window initialized successfully, entering main loop");
    Setup(); // Call the user-defined setup function

    isRunning_ = true;
    int frameCount = 0;
    while (isRunning_ && !WindowShouldClose())
    {
        frameCount++;

        // Update input and time managers
        input_.Update();
        time_.Update();

        // Call the user-defined update function before drawing (for game logic, physics, etc.)
        Update();

        // Call the user-defined draw function within the raylib drawing context
        BeginDrawing();

        // Call the user-defined draw function to render the frame
        Draw();

        // End the drawing for this frame
        EndDrawing();

        // Each 300 frames, execute the logic if needed (e.g., flush logs, perform maintenance tasks)
        if (frameCount % 300 == 0)
            RS::Logger::Flush();
    }

    // Ensure flag is cleared when exiting the loop
    isRunning_ = false;

    LOG_INFO("Window close requested, shutting down...");

    CloseWindow();

    LOG_INFO("Shutdown complete");

    // Final flush to ensure all logs are written before exit
    RS::Logger::Flush();
}
