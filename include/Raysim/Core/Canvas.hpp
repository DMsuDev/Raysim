#pragma once

#include <string>
#include "RaySim/Math/Vector.hpp"
#include "RaySim/Graphics/Color.hpp"

namespace RS
{

    class Canvas
    {
    public:
        virtual ~Canvas() = default;

        virtual void Setup() {} // Set up the canvas (called once at the start of the simulation)
        virtual void Draw() {}  // Draw the canvas (called every frame during the simulation)

        void SetSize(int width, int height);
        void SetTitle(const std::string &title);
        void SetFrameRate(int fps);
        void FullScreen();

        void Run(); // Start the simulation loop

        RS::Vec2 GetScreenSize() const { return {static_cast<float>(width_), static_cast<float>(height_)}; }
        int GetWidth() const { return width_; }
        int GetHeight() const { return height_; }

        // Utility drawing functions to implement with raylib

        void Background(const RS::Color &color);
        void DrawText(const char *text, int posX, int posY, int fontSize, const RS::Color &color);

    private:
        int fps_               {60};
        int width_             {800};
        int height_            {600};
        std::string title_     {"RaySim Title"};
    };

} // namespace RS
