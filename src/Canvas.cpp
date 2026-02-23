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

void RS::Canvas::SetDefaultFont(const std::string& fontPath, int fontSize)
{
    fontManager_.LoadFont(fontPath, fontSize);
}

void RS::Canvas::Background(const RS::RColor &color)
{
    ::ClearBackground(color.raylib());
}

void RS::Canvas::DrawText(const char *text, int posX, int posY, int fontSize, const RS::RColor &color)
{
    // Use loaded font if available, otherwise fall back to raylib default
    if (fontManager_.HasFont()) {
        DrawTextEx(text, static_cast<float>(posX), static_cast<float>(posY), fontManager_.GetFont(), static_cast<float>(fontSize), color);
    } else {
        ::DrawText(text, posX, posY, fontSize, color.raylib());
    }
}

void RS::Canvas::DrawTextEx(const char *text, float posX, float posY, const FontHandle& font, float fontSize, const RS::RColor &color)
{
    if (font.IsValid()) {
        ::Font* fontPtr = static_cast<::Font*>(font.data);
        ::DrawTextEx(*fontPtr, text, {posX, posY}, fontSize, 1.0f, color.raylib());
    } else {
        // Fallback to default raylib font
        ::DrawText(text, static_cast<int>(posX), static_cast<int>(posY), static_cast<int>(fontSize), color.raylib());
    }
}

void RS::Canvas::Run()
{

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
