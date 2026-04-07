#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

using namespace RS;

class MouseDetection : public Application {

    // Convenience accessors
    uint32_t GetWidth()  const { return GetContext().Window->GetWidth(); }
    uint32_t GetHeight() const { return GetContext().Window->GetHeight(); }

private:
    Vector2 mousePos{0.0f, 0.0f};
    Vector2 mouseSmoothedPos{0.0f, 0.0f};
    float squareSpeed = 10.0f;

    // Mouse button state captured in Update for Draw
    bool leftDown_   = false;
    bool rightDown_  = false;
    bool middleDown_ = false;

public:

    void OnStart() override {
        GetContext().Window->SetWindowSize(1000, 600);
        GetContext().Window->SetWindowTitle("Raysim - Mouse Detection Demo");
        FontManager::LoadFont("assets/fonts/OpenSans-Regular.ttf");
        Time::SetTargetFPS(60);
    }

    void OnUpdate(float dt) override {
        mousePos        = GetContext().Input->GetMousePosition();
        mouseSmoothedPos = Math::Lerp(mouseSmoothedPos, mousePos, squareSpeed * dt);

        leftDown_   = GetContext().Input->IsMouseButtonDown(MouseButton::Left);
        rightDown_  = GetContext().Input->IsMouseButtonDown(MouseButton::Right);
        middleDown_ = GetContext().Input->IsMouseButtonDown(MouseButton::Middle);
    }

    void OnDraw(float /*alpha*/) override {
        GetContext().Renderer->ClearScreen(Colors::DarkBlue);

        float w = static_cast<float>(GetWidth());
        float h = static_cast<float>(GetHeight());

        // Draw grid background
        Color gridColor{60, 60, 60};
        for (int x = 0; x < GetWidth(); x += 50) {
            Shapes::DrawLine(static_cast<float>(x), 0.0f,
                             static_cast<float>(x), h, gridColor);
        }
        for (int y = 0; y < GetHeight(); y += 50) {
            Shapes::DrawLine(0.0f, static_cast<float>(y),
                             w, static_cast<float>(y), gridColor);
        }

        // Left rectangle - scales with mouse Y
        float fixedY = h * 0.5f;
        float baseSize = 15.0f;
        float mouseScaledY = mousePos.y / 2.0f;
        float sizeL = mouseScaledY + baseSize;

        Color rectColorL = { 239, 68, 68, 200 };
        Shapes::DrawRect(mouseSmoothedPos.x, fixedY, sizeL, sizeL, rectColorL, OriginMode::Center);

        // Right rectangle - inverted scale
        Color rectColorR = {  0, 161, 224, 200 };
        float invertedSize = ((h - mousePos.y) / 2.0f) + baseSize;
        Shapes::DrawRect(w - mouseSmoothedPos.x, fixedY, invertedSize, invertedSize, rectColorR, OriginMode::Center);

        // Center circle following mouse
        Shapes::DrawCircle(mousePos.x, mousePos.y, 8.0f, Colors::RayWhite, OriginMode::Center);

        // Mouse position text
        std::string posText = "Mouse: (" + std::to_string(static_cast<int>(mousePos.x)) +
                             ", " + std::to_string(static_cast<int>(mousePos.y)) + ")";
        Text::DrawText(posText, 50, 50, 24, Colors::White, OriginMode::TopLeft);

        // Instructions
        Text::DrawText("Move your mouse! RMB, LMB or MMB to interact",
                       50, h - 50.0f, 21, Colors::LightGray, OriginMode::BottomLeft);

        // Draw FPS
        std::string fpsText = "FPS: " + std::to_string(static_cast<int>(Time::GetSmoothedFPS()));
        Text::DrawText(fpsText, w - 50.0f, 50, 24, Colors::LightGray, OriginMode::TopRight);

        // Color blocks on mouse buttons
        float btnY = 100.0f;
        if (leftDown_) {
            Shapes::DrawRect(w * 0.2f, btnY, 80, 80, Colors::RayRed, OriginMode::Center);
            Text::DrawText("LEFT", w * 0.2f, btnY, 20, Colors::RayBlack, OriginMode::Center);
        }
        if (rightDown_) {
            Shapes::DrawRect(w * 0.8f, btnY, 80, 80, Colors::RayGreen, OriginMode::Center);
            Text::DrawText("RIGHT", w * 0.8f, btnY, 20, Colors::RayBlack, OriginMode::Center);
        }
        if (middleDown_) {
            Shapes::DrawRect(w * 0.5f, btnY, 80, 80, Colors::RayBlue, OriginMode::Center);
            Text::DrawText("MIDDLE", w * 0.5f, btnY, 20, Colors::RayBlack, OriginMode::Center);
        }
    }
};

//==============================================================================
// Entry point
//==============================================================================

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    return new MouseDetection();
}
