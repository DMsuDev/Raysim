#include "Raysim/Raysim.hpp"

using namespace RS;

/**
 * @brief Simple 2D mouse detection demo
 *
 * This example demonstrates how to use the InputManager to track mouse position and button states.
 * It draws rectangles that scale based on the mouse's Y position and changes color when mouse buttons
 * are pressed. The mouse position is also displayed as text on the screen.
 *
 * Controls:
 *
 * - Move mouse to see rectangles scale and follow the cursor
 *
 * - Left Click: Red rectangle appears
 *
 * - Right Click: Green rectangle appears
 *
 * - Middle Click: Blue rectangle appears
 * @see InputManager, TimeManager, Text
 */
class MouseDetection : public Canvas {
public:

    void Setup() override {
        SetSize(1000, 600);
        SetTitle("RaySim - Mouse Detection Demo");
        SetDefaultFont("resources/fonts/opensans/OpenSans-Regular.ttf");
        SetTargetFPS(60);
    }

    void Draw() override {
        Background(Color::DarkBlue());

        // Draw grid background
        Color gridColor{60, 60, 60};
        for (int x = 0; x < GetWidth(); x += 50) {
            Shapes::DrawLine((float)x, 0, (float)x, (float)GetHeight(), gridColor);
        }
        for (int y = 0; y < GetHeight(); y += 50) {
            Shapes::DrawLine(0, (float)y, (float)GetWidth(), (float)y, gridColor);
        }

        InputManager& input = GetInputManager();
        Vector2 mousePos = input.GetMousePosition();

        // Left rectangle - scales with mouse Y
        float fixedY = 300.0f;
        float baseSize = 15.0f;
        float mouseScaledY = mousePos.y / 2.0f;
        float sizeL = mouseScaledY + baseSize;

        Color rectColorL = { 239, 68, 68, 200 }; // Semi-transparent red
        Shapes::DrawRect(mousePos.x, fixedY, sizeL, sizeL, rectColorL, OriginMode::Center);

        // Right rectangle - inverted scale
        Color rectColorR = {  0, 161, 224, 200 }; // Semi-transparent blue
        float invertedSize = ((600.0f - mousePos.y) / 2.0f) + baseSize;
        Shapes::DrawRect(1000.0f - mousePos.x, fixedY, invertedSize, invertedSize, rectColorR, OriginMode::Center);

        // Center circles following mouse
        Shapes::DrawCircle(mousePos.x, mousePos.y, 8, Color::RayWhite(), OriginMode::Center);

        // Mouse position text
        std::string posText = "Mouse: (" + std::to_string(static_cast<int>(mousePos.x)) +
                             ", " + std::to_string(static_cast<int>(mousePos.y)) + ")";
        Text::DrawText(posText, 50, 50, 24, Color::White(), OriginMode::TopLeft);

        // Instructions
        Text::DrawText("Move your mouse! Left Click for colors", 50, 550, 21, Color::LightGray(), OriginMode::BottomLeft);

        // Draw FPS
        std::string fpsText = "FPS: " + std::to_string(GetTimeManager().GetCurrentFPS());
        Text::DrawText(fpsText, 950, 50, 24, Color::LightGray(), OriginMode::TopRight);

        // Color blocks on mouse buttons
        if (input.IsMouseButtonDown(MouseButton::Left)) {
            Shapes::DrawRect(200, 100, 80, 80, Color::RayRed(), OriginMode::Center);
            Text::DrawText("LEFT", 200, 100, 20, Color::RayBlack(), OriginMode::Center);
        }
        if (input.IsMouseButtonDown(MouseButton::Right)) {
            Shapes::DrawRect(800, 100, 80, 80, Color::RayGreen(), OriginMode::Center);
            Text::DrawText("RIGHT", 800, 100, 20, Color::RayBlack(), OriginMode::Center);
        }
        if (input.IsMouseButtonDown(MouseButton::Middle)) {
            Shapes::DrawRect(500, 100, 80, 80, Color::RayBlue(), OriginMode::Center);
            Text::DrawText("MIDDLE", 500, 100, 20, Color::RayBlack(), OriginMode::Center);
        }
    }
};

int main() {
    MouseDetection sim;
    sim.Run();
    return 0;
}
