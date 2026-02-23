#include "RaySim/Core/Canvas.hpp"
#include "pch.hpp"

#include <raylib.h>

void RS::Canvas::SetSize(int width, int height)
{
    width_ = width;
    height_ = height;

    // If the window is already created, update the size
    if (IsWindowReady()) { SetWindowSize(width_, height_); }
}

void RS::Canvas::SetTitle(const std::string& title)
{
    title_ = title;
    // If the window is already created, update the title
    if (IsWindowReady()) { SetWindowTitle(title_.c_str()); }
}

void RS::Canvas::SetFrameRate(int fps)
{
    fps_ = fps;
    // If the window is already created, update the target FPS
    if (IsWindowReady()) { SetTargetFPS(fps_); }
}

void RS::Canvas::FullScreen()
{
    SetSize(GetMonitorWidth(0), GetMonitorHeight(0));
    ::ToggleFullscreen();
}

void RS::Canvas::Background(const RS::Color &color)
{
    ::ClearBackground({color.r, color.g, color.b, color.a});
}

void RS::Canvas::DrawText(const char *text, int posX, int posY, int fontSize, const RS::Color &color)
{
    ::DrawText(text, posX, posY, fontSize, {color.r, color.g, color.b, color.a});
}

void RS::Canvas::Run()
{
    // Reduce raylib log verbosity to warnings and errors only
    SetTraceLogLevel(LOG_WARNING);

    InitWindow(width_, height_, title_.c_str());
    SetTargetFPS(fps_);

    Setup(); // Call the user-defined setup function
    while (!WindowShouldClose())
    {
        // Call the user-defined draw function within the raylib drawing context
        BeginDrawing();

        // Call the user-defined draw function to render the frame
        Draw();

        // End the drawing for this frame
        EndDrawing();
    }
    CloseWindow();
}
