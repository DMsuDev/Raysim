#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

#include "example_layers.hpp"

using namespace RS;

class MouseDetection : public Scene {
    RS_SCENE(MouseDetection)

private:
    Math::Vec2f mousePos{0.0f, 0.0f};
    Math::Vec2f offset{0.0f, 0.0f};
    Math::Vec2f mouseSmoothedPos{0.0f, 0.0f};

    static constexpr float SQUARE_SPEED = 10.0f;
    static constexpr float MIN_SQUARE_SIZE = 30.0f;

    static constexpr float OFFSET_MOVE_X = 100.0f;
    static constexpr float OFFSET_MOVE_Y = 100.0f;

    bool leftDown_   = false;
    bool rightDown_  = false;
    bool middleDown_ = false;

    void DrawGridMovement(Math::Vec2f offset, float cellSize, Math::Vec2f windowSize) {

        float startX = Math::Mod(offset.x, cellSize);
        float startY = Math::Mod(offset.y, cellSize);

        // Draw grid background
        Color gridColor{60, 60, 60};
        for (float  x = startX; x < windowSize.x; x += cellSize)
            Shapes::DrawLine(x, 0.0f, x, windowSize.y, gridColor);
        for (float  y = startY; y < windowSize.y; y += cellSize)
            Shapes::DrawLine(0.0f, y, windowSize.x, y, gridColor);
    }

public:

    void OnStart() override {
        FontManager::LoadFont("opensans", "fonts/OpenSans-Regular.ttf", 32);

        // ImGui overlay
        SetupImGuiLayer();

        // Push debug layers
        PushLayer(new MouseInfoLayer(mousePos, mouseSmoothedPos));
        PushLayer(new FpsLayer(window().GetWidth()));
        PushLayer(new InstructionsLayer(window().GetHeight()));
        PushLayer(new MouseButtonsLayer(window().GetSize(), leftDown_, rightDown_, middleDown_));
    }

    void OnUpdate(float dt) override {
        mousePos = input().GetMousePosition();
        mouseSmoothedPos = Math::Lerp(mouseSmoothedPos, mousePos, SQUARE_SPEED * dt);

        offset.x += OFFSET_MOVE_X * dt;
        offset.y += OFFSET_MOVE_Y * dt;

        leftDown_   = input().IsMouseButtonDown(MouseButton::Left);
        rightDown_  = input().IsMouseButtonDown(MouseButton::Right);
        middleDown_ = input().IsMouseButtonDown(MouseButton::Middle);
    }

    void OnDraw(float alpha) override {
        renderer().ClearScreen(Colors::DarkBlue);

        float w = static_cast<float>(window().GetWidth());
        float h = static_cast<float>(window().GetHeight());

        DrawGridMovement(offset, 50.0f, {w,h});

        // Left rectangle - scales with mouse Y
        float fixedY   = h * 0.5f;
        float baseSize = 15.0f;
        float sizeL    = Math::MaxValue((mousePos.y) / 2.0f + baseSize, MIN_SQUARE_SIZE);

        Color rectColorL = { 239, 68, 68, 200 };
        Shapes::DrawRect(mouseSmoothedPos.x, fixedY, sizeL, sizeL, rectColorL, OriginMode::Center);

        // Right rectangle - inverted scale
        Color rectColorR = { 0, 161, 224, 200 };
        float invertedSize = Math::MaxValue(((h - mousePos.y) / 2.0f) + baseSize, MIN_SQUARE_SIZE);
        Shapes::DrawRect(w - mouseSmoothedPos.x, fixedY, invertedSize, invertedSize, rectColorR, OriginMode::Center);

        // Center circle following mouse
        Shapes::DrawCircle(mousePos.x, mousePos.y, 8.0f, Colors::RayWhite, OriginMode::Center);

        // Color blocks on mouse buttons
        float btnY = 100.0f;
        if (leftDown_) {
            Shapes::DrawRect(w * 0.2f, btnY, 80, 80, Colors::RayRed, OriginMode::Center);
            Text::RenderText("LEFT", w * 0.2f, btnY, 20, Colors::RayBlack, OriginMode::Center);
        }
        if (rightDown_) {
            Shapes::DrawRect(w * 0.8f, btnY, 80, 80, Colors::RayGreen, OriginMode::Center);
            Text::RenderText("RIGHT", w * 0.8f, btnY, 20, Colors::RayBlack, OriginMode::Center);
        }
        if (middleDown_) {
            Shapes::DrawRect(w * 0.5f, btnY, 80, 80, Colors::RayBlue, OriginMode::Center);
            Text::RenderText("MIDDLE", w * 0.5f, btnY, 20, Colors::RayBlack, OriginMode::Center);
        }
    }
};

//==============================================================================
// Entry point
//==============================================================================

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    RS::ApplicationConfig config;
    config.Window.Title  = "Raysim - Mouse Detection Demo";
    config.Window.Width  = 1000;
    config.Window.Height = 600;
    config.TargetFPS     = 60;
    config.VSync         = true;

    auto* app = new RS::Application(config);
    app->RegisterScene<MouseDetection>();
    app->ChangeScene<MouseDetection>();
    return app;
}
