#include "Raysim/Raysim.hpp"

using namespace RS;

class MouseDetection : public Application {

private:
    Vector2 mousePos{0.0f, 0.0f};
    Vector2 mouseSmootedPos{0.0f, 0.0f};
    float squareSpeed = 10.0f; // Speed at which the squares follow the mouse

public:

    void Setup() override {
        SetSize(1000, 600);
        SetTitle("Raysim - Mouse Detection Demo");
        SetDefaultFont("assets/fonts/OpenSans-Regular.ttf");
        Time::SetTargetFPS(60);
    }

    void Draw(float alpha) override {
        Background(Colors::DarkBlue);

        // Draw grid background
        Color gridColor{60, 60, 60};
        for (int x = 0; x < GetWidth(); x += 50) {
            Shapes::DrawLine((float)x, 0, (float)x, (float)GetHeight(), gridColor);
        }
        for (int y = 0; y < GetHeight(); y += 50) {
            Shapes::DrawLine(0, (float)y, (float)GetWidth(), (float)y, gridColor);
        }

        mousePos = Input->GetMousePosition();
        mouseSmootedPos = Math::Lerp(mouseSmootedPos, mousePos, squareSpeed * Time::GetDeltaTime()); // Smooth mouse movement

        // Left rectangle - scales with mouse Y
        float fixedY = 300.0f;
        float baseSize = 15.0f;
        float mouseScaledY = mousePos.y / 2.0f;
        float sizeL = mouseScaledY + baseSize;

        Color rectColorL = { 239, 68, 68, 200 }; // Semi-transparent red
        Shapes::DrawRect(mouseSmootedPos.x, fixedY, sizeL, sizeL, rectColorL, OriginMode::Center);

        // Right rectangle - inverted scale
        Color rectColorR = {  0, 161, 224, 200 }; // Semi-transparent blue
        float invertedSize = ((600.0f - mousePos.y) / 2.0f) + baseSize;
        Shapes::DrawRect(1000.0f - mouseSmootedPos.x, fixedY, invertedSize, invertedSize, rectColorR, OriginMode::Center);

        // Center circles following mouse
        Shapes::DrawCircle(mousePos.x, mousePos.y, 8, Colors::RayWhite, OriginMode::Center);

        // Mouse position text
        std::string posText = "Mouse: (" + std::to_string(static_cast<int>(mousePos.x)) +
                             ", " + std::to_string(static_cast<int>(mousePos.y)) + ")";
        Text::DrawText(posText, 50, 50, 24, Colors::White, OriginMode::TopLeft);

        // Instructions
        Text::DrawText("Move your mouse! RMB, LMB or MMB to interact", 50, 550, 21, Colors::LightGray, OriginMode::BottomLeft);

        // Draw FPS
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(Time::GetSmoothedFPS()));
        Text::DrawText(fpsText, 950, 50, 24, Colors::LightGray, OriginMode::TopRight);

        // Color blocks on mouse buttons
        if (Input->IsMouseButtonDown(MouseButton::Left)) {
            Shapes::DrawRect(200, 100, 80, 80, Colors::RayRed, OriginMode::Center);
            Text::DrawText("LEFT", 200, 100, 20, Colors::RayBlack, OriginMode::Center);
        }
        if (Input->IsMouseButtonDown(MouseButton::Right)) {
            Shapes::DrawRect(800, 100, 80, 80, Colors::RayGreen, OriginMode::Center);
            Text::DrawText("RIGHT", 800, 100, 20, Colors::RayBlack, OriginMode::Center);
        }
        if (Input->IsMouseButtonDown(MouseButton::Middle)) {
            Shapes::DrawRect(500, 100, 80, 80, Colors::RayBlue, OriginMode::Center);
            Text::DrawText("MIDDLE", 500, 100, 20, Colors::RayBlack, OriginMode::Center);
        }
    }
};

int main() {
    MouseDetection sim;
    sim.Run();
    return 0;
}
