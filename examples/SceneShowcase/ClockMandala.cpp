#include "ClockMandala.hpp"

#include "StarField.hpp"
#include "PlasmaArt.hpp"

#include <cmath>

// ============================================================================
// MandalaControlLayer
// ============================================================================

void MandalaControlLayer::OnUIRender()
{
    ImGui::SetNextWindowPos(ImVec2(10.0f, 60.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.65f);
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    ImGui::Begin("##mandctrl", nullptr, flags);
    ImGui::TextUnformatted("Mandala Controls");
    ImGui::Separator();
    ImGui::SliderInt("Rings",   &m_Rings, 2, 8);
    ImGui::SliderFloat("Speed", &m_Speed, 0.1f, 4.0f);
    ImGui::End();
}

// ============================================================================
// ClockMandala
// ============================================================================

void ClockMandala::OnStart()
{
    GetWindow().SetSize(1000, 700);
    GetWindow().SetTitle("SceneShowcase - ClockMandala");
    Time::SetTargetFPS(60);

    SetupImGuiLayer();
    PushLayer(new MandalaControlLayer(m_Rings, m_Speed));
    PushOverlay(new SceneNavLayer("ClockMandala", 3, 3,
        [this]() { GetSceneManager().ChangeScene<StarField>(); },
        [this]() { GetSceneManager().ChangeScene<PlasmaArt>(); }
    ));
}

void ClockMandala::OnUpdate(float dt)
{
    m_Time += dt * m_Speed;
}

void ClockMandala::OnDraw(float /*alpha*/)
{
    GetRenderer().ClearScreen(Color{10, 5, 30});

    float w  = static_cast<float>(GetWindow().GetWidth());
    float h  = static_cast<float>(GetWindow().GetHeight());
    float cx = w * 0.5f;
    float cy = h * 0.5f;

    float maxR = Math::MinValue(w, h) * 0.44f;

    for (int ring = 1; ring <= m_Rings; ++ring)
    {
        float t      = static_cast<float>(ring) / static_cast<float>(m_Rings);
        float radius = t * maxR;
        int   nodes  = (ring + 1) * 6;
        float dir    = (ring % 2 == 0) ? 1.0f : -1.0f;
        float rot    = m_Time * dir * (0.4f + 0.15f * ring);
        float hue    = std::fmod(t * 240.0f + m_Time * 25.0f, 360.0f);

        Color cBright = Color::FromHSV(hue, 0.95f, 1.0f, 220);
        Color cDim    = Color::FromHSV(hue, 0.60f, 0.55f, 100);

        Shapes::DrawCircleOutline(cx, cy, radius, cDim, OriginMode::Center);

        int   sides    = 3 + (ring % 4);
        float nodeSize = 6.0f + (1.0f - t) * 14.0f;
        for (int n = 0; n < nodes; ++n)
        {
            float angle  = rot + n * (Math::TWO_PI / static_cast<float>(nodes));
            float px     = cx + std::cos(angle) * radius;
            float py     = cy + std::sin(angle) * radius;
            float rotDeg = Math::ToDegrees(angle) + m_Time * 60.0f * dir;
            Shapes::DrawPolygon(px, py, sides, nodeSize * 0.5f, rotDeg, cBright);
        }
    }

    // Pulsing starburst at the centre
    float pulse = 28.0f + std::sin(m_Time * 3.0f) * 8.0f;
    for (int i = 0; i < 16; ++i)
    {
        float angle = m_Time * 0.6f + i * (Math::TWO_PI / 16.0f);
        float len   = pulse + std::sin(m_Time * 5.0f + i * 0.7f) * 6.0f;
        float ex    = cx + std::cos(angle) * len;
        float ey    = cy + std::sin(angle) * len;
        Color lc    = Color::FromHSV(std::fmod(m_Time * 80.0f + i * 22.5f, 360.0f), 1.0f, 1.0f);
        Shapes::DrawLine(cx, cy, ex, ey, lc, 2.0f);
    }

    Shapes::DrawCircle(cx, cy, 6.0f, Colors::White, OriginMode::Center);
}
