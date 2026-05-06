#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

using namespace RS;

// ============================================================================
// Debug overlay layers
// ============================================================================

/// Top-left: mouse position and smoothed position
class MouseInfoLayer : public Layer
{
public:
    MouseInfoLayer(const Math::Vec2& mousePos, const Math::Vec2& smoothedPos)
        : Layer("MouseInfoLayer"), m_MousePos(mousePos), m_SmoothedPos(smoothedPos) {}

    void OnUIRender() override
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(10.0f, 10.0f), ImGuiCond_Always);
        ImGui::SetNextWindowBgAlpha(0.55f);
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

        ImGui::Begin("##mouseinfo", nullptr, flags);
        ImGui::Text("Mouse     : (%d, %d)", (int)m_MousePos.x, (int)m_MousePos.y);
        ImGui::Text("Smoothed  : (%d, %d)", (int)m_SmoothedPos.x, (int)m_SmoothedPos.y);
        ImGui::End();
    }

private:
    const Math::Vec2& m_MousePos;
    const Math::Vec2& m_SmoothedPos;
};

/// Top-right: FPS counter
class FpsLayer : public Layer
{
public:
    FpsLayer(int winWidth)
        : Layer("FpsLayer"), m_WinWidth(winWidth) {}

    void OnUIRender() override
    {
        float padding = 10.0f;
        ImGui::SetNextWindowPos(ImVec2((float)m_WinWidth - padding, padding), ImGuiCond_Always, ImVec2(1.0f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.55f);
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

        ImGui::Begin("##fps", nullptr, flags);
        ImGui::Text("FPS: %d", (int)Time::GetSmoothedFPS());
        ImGui::End();
    }

private:
    int m_WinWidth;
};

/// Bottom-left: instructions
class InstructionsLayer : public Layer
{
public:
    InstructionsLayer(int winHeight)
        : Layer("InstructionsLayer"), m_WinHeight(winHeight) {}

    void OnUIRender() override
    {
        float padding = 10.0f;
        ImGui::SetNextWindowPos(ImVec2(padding, (float)m_WinHeight - padding), ImGuiCond_Always, ImVec2(0.0f, 1.0f));
        ImGui::SetNextWindowBgAlpha(0.55f);
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

        ImGui::Begin("##instructions", nullptr, flags);
        ImGui::TextUnformatted("Move your mouse! LMB / RMB / MMB to interact");
        ImGui::End();
    }

private:
    int m_WinHeight;
};

/// Bottom-right: mouse button state + interactive log button
class MouseButtonsLayer : public Layer
{
public:
    MouseButtonsLayer(int winWidth, int winHeight,
                      const bool& left, const bool& right, const bool& middle)
        : Layer("MouseButtonsLayer")
        , m_WinWidth(winWidth), m_WinHeight(winHeight)
        , m_Left(left), m_Right(right), m_Middle(middle) {}

    void OnUIRender() override
    {
        float padding = 10.0f;
        ImGui::SetNextWindowPos(
            ImVec2((float)m_WinWidth - padding, (float)m_WinHeight - padding),
            ImGuiCond_Always, ImVec2(1.0f, 1.0f));
        ImGui::SetNextWindowBgAlpha(0.55f);
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

        ImGui::Begin("##buttons", nullptr, flags);

        ImGui::Text("LMB: %s", m_Left   ? "DOWN" : "up");
        ImGui::Text("RMB: %s", m_Right  ? "DOWN" : "up");
        ImGui::Text("MMB: %s", m_Middle ? "DOWN" : "up");

        ImGui::Spacing();
        if (ImGui::Button("Log click"))
            RS_INFO("Mouse2D: 'Log click' button pressed");

        ImGui::End();
    }

private:
    int         m_WinWidth;
    int         m_WinHeight;
    const bool& m_Left;
    const bool& m_Right;
    const bool& m_Middle;
};

// ============================================================================
// Scene
// ============================================================================

class MouseDetection : public Scene {
    RS_SCENE(MouseDetection)

private:
    int GetWidth()  { return GetWindow().GetWidth(); }
    int GetHeight() { return GetWindow().GetHeight(); }

    Math::Vec2 mousePos{0.0f, 0.0f};
    Math::Vec2 mouseSmoothedPos{0.0f, 0.0f};
    float squareSpeed = 10.0f;

    bool leftDown_   = false;
    bool rightDown_  = false;
    bool middleDown_ = false;

public:

    void OnStart() override {
        GetWindow().SetSize(1000, 600);
        GetWindow().SetTitle("Raysim - Mouse Detection Demo");
        FontManager::LoadFont("opensans", "fonts/OpenSans-Regular.ttf", 32);
        Time::SetTargetFPS(60);

        // ImGui overlay
        SetupImGuiLayer();

        // Push debug layers
        PushLayer(new MouseInfoLayer(mousePos, mouseSmoothedPos));
        PushLayer(new FpsLayer(GetWidth()));
        PushLayer(new InstructionsLayer(GetHeight()));
        PushLayer(new MouseButtonsLayer(GetWidth(), GetHeight(),
                                        leftDown_, rightDown_, middleDown_));
    }

    void OnUpdate(float dt) override {
        mousePos = GetInput().GetMousePosition();
        mouseSmoothedPos = Math::Lerp(mouseSmoothedPos, mousePos, squareSpeed * dt);

        leftDown_   = GetInput().IsMouseButtonDown(MouseButton::Left);
        rightDown_  = GetInput().IsMouseButtonDown(MouseButton::Right);
        middleDown_ = GetInput().IsMouseButtonDown(MouseButton::Middle);
    }

    void OnDraw(float /*alpha*/) override {
        GetRenderer().ClearScreen(Colors::DarkBlue);

        float w = static_cast<float>(GetWidth());
        float h = static_cast<float>(GetHeight());

        // Draw grid background
        Color gridColor{60, 60, 60};
        for (int x = 0; x < GetWidth(); x += 50)
            Shapes::DrawLine(static_cast<float>(x), 0.0f, static_cast<float>(x), h, gridColor);
        for (int y = 0; y < GetHeight(); y += 50)
            Shapes::DrawLine(0.0f, static_cast<float>(y), w, static_cast<float>(y), gridColor);

        // Left rectangle - scales with mouse Y
        float fixedY    = h * 0.5f;
        float baseSize  = 15.0f;
        float sizeL     = mousePos.y / 2.0f + baseSize;
        Color rectColorL = { 239, 68, 68, 200 };
        Shapes::DrawRect(mouseSmoothedPos.x, fixedY, sizeL, sizeL, rectColorL, OriginMode::Center);

        // Right rectangle - inverted scale
        Color rectColorR = { 0, 161, 224, 200 };
        float invertedSize = ((h - mousePos.y) / 2.0f) + baseSize;
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

    auto* app = new RS::Application(config);
    app->RegisterScene<MouseDetection>();
    app->ChangeScene<MouseDetection>();
    return app;
}
