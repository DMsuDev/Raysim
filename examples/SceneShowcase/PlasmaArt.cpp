#include "PlasmaArt.hpp"

#include "StarField.hpp"
#include "ClockMandala.hpp"

#include <cmath>

// ============================================================================
// PlasmaControlLayer
// ============================================================================

void PlasmaControlLayer::OnUIRender()
{
    ImGui::SetNextWindowPos(ImVec2(10.0f, 60.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.65f);
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    ImGui::Begin("##plactrl", nullptr, flags);
    ImGui::TextUnformatted("Plasma Settings");
    ImGui::Separator();
    ImGui::SliderFloat("Freq X", &m_FX,    0.5f, 8.0f);
    ImGui::SliderFloat("Freq Y", &m_FY,    0.5f, 8.0f);
    ImGui::SliderFloat("Speed",  &m_Speed, 0.1f, 5.0f);
    ImGui::End();
}

// ============================================================================
// PlasmaArt
// ============================================================================

void PlasmaArt::OnStart()
{
    GetWindow().SetSize(1000, 700);
    GetWindow().SetTitle("SceneShowcase - PlasmaArt");
    Time::SetTargetFPS(60);

    SetupImGuiLayer();
    PushLayer(new PlasmaControlLayer(m_FX, m_FY, m_Speed));
    PushOverlay(new SceneNavLayer("PlasmaArt", 2, 3,
        [this]() { GetSceneManager().ChangeScene<ClockMandala>(); },
        [this]() { GetSceneManager().ChangeScene<StarField>(); }
    ));
}

void PlasmaArt::OnUpdate(float dt)
{
    m_Time += dt * m_Speed;
}

void PlasmaArt::OnDraw(float /*alpha*/)
{
    GetRenderer().ClearScreen(Colors::Black);

    float cw = static_cast<float>(GetWindow().GetWidth())  / COLS;
    float ch = static_cast<float>(GetWindow().GetHeight()) / ROWS;

    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            float nx = static_cast<float>(col) / COLS;
            float ny = static_cast<float>(row) / ROWS;

            float v = std::sin(nx * m_FX  * Math::PI + m_Time)
                    + std::sin(ny * m_FY  * Math::PI + m_Time * 0.7f)
                    + std::sin((nx + ny)  * 3.0f     + m_Time * 1.3f)
                    + std::sin(std::sqrt(nx * nx + ny * ny) * 5.0f - m_Time);

            float hue = std::fmod((v + 4.0f) * 45.0f, 360.0f);
            Color c   = Color::FromHSV(hue, 1.0f, 1.0f);

            Shapes::DrawRect(col * cw, row * ch, cw + 1.0f, ch + 1.0f, c);
        }
    }
}
