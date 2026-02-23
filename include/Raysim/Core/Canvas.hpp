#pragma once

#include <string>

#include "Raysim/Math/Vector.hpp"
#include "Raysim/Graphics/Color.hpp"
#include "Raysim/Graphics/FontManager.hpp"

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

        // Font management
        void SetDefaultFont(const std::string& fontPath, int fontSize = 32);
        FontManager& GetFontManager() { return fontManager_; }

        void Run(); // Start the simulation loop

        RS::Vec2 GetScreenSize() const { return {static_cast<float>(width_), static_cast<float>(height_)}; }
        int GetWidth() const { return width_; }
        int GetHeight() const { return height_; }

        // Utility drawing functions to implement with raylib
        void Background(const RS::RColor &color);
        void DrawText(const char *text, int posX, int posY, int fontSize, const RS::RColor &color);
        void DrawTextEx(const char *text, float posX, float posY, const FontHandle& font, float fontSize, const RS::RColor &color);

    private:
        int fps_               {60};
        int width_             {800};
        int height_            {600};
        std::string title_     {"RaySim Title"};

        FontManager fontManager_;
    };

} // namespace RS
