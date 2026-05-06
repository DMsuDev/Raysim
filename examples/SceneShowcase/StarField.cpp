#include "StarField.hpp"

#include "PlasmaArt.hpp"
#include "ClockMandala.hpp"

#include <cmath>

// ============================================================================
// StarFieldControlLayer
// ============================================================================

void StarFieldControlLayer::OnUIRender()
{
    ImGui::SetNextWindowPos(ImVec2(10.0f, 60.0f), ImGuiCond_Always);
    ImGui::SetNextWindowBgAlpha(0.65f);
    ImGuiWindowFlags flags =
        ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
        ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
        ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

    ImGui::Begin("##sfctrl", nullptr, flags);
    ImGui::TextUnformatted("Warp Controls");
    ImGui::Separator();
    ImGui::SliderFloat("Speed",        &m_Speed,   0.1f, 6.0f);
    ImGui::Checkbox("Rainbow stars",   &m_Rainbow);
    ImGui::End();
}

// ============================================================================
// StarField
// ============================================================================

void StarField::Reset(Star& s, bool spread)
{
    s.x   = Math::Random::Range(-1.0f, 1.0f);
    s.y   = Math::Random::Range(-1.0f, 1.0f);
    s.z   = spread ? Math::Random::Range(0.01f, 1.0f) : 1.0f;
    s.hue = Math::Random::Range(0.0f, 360.0f);
}

void StarField::OnStart()
{
    GetWindow().SetSize(1000, 700);
    GetWindow().SetTitle("SceneShowcase - StarField");
    Time::SetTargetFPS(60);

    m_Stars.resize(NUM_STARS);
    for (auto& s : m_Stars) Reset(s, true);

    SetupImGuiLayer();
    PushLayer(new StarFieldControlLayer(m_Speed, m_Rainbow));
    PushOverlay(new SceneNavLayer("StarField", 1, 3,
        [this]() { GetSceneManager().ChangeScene<PlasmaArt>(); },
        [this]() { GetSceneManager().ChangeScene<ClockMandala>(); }
    ));
}

void StarField::OnUpdate(float dt)
{
    float step = BASE_SPEED * m_Speed * dt;
    for (auto& s : m_Stars)
    {
        s.z -= step;
        if (s.z <= 0.0f) Reset(s, false);
    }
}

void StarField::OnDraw(float /*alpha*/)
{
    GetRenderer().ClearScreen(Color{5, 5, 18});

    float hw = GetWindow().GetWidth()  * 0.5f;
    float hh = GetWindow().GetHeight() * 0.5f;
    float sw = static_cast<float>(GetWindow().GetWidth());
    float sh = static_cast<float>(GetWindow().GetHeight());

    for (const auto& s : m_Stars)
    {
        float sx = (s.x / s.z) * hw + hw;
        float sy = (s.y / s.z) * hh + hh;
        if (sx < 0.0f || sx >= sw || sy < 0.0f || sy >= sh) continue;

        float t             = 1.0f - s.z;
        float size          = t * 5.5f + 0.5f;
        unsigned char alpha = static_cast<unsigned char>(t * 255.0f);

        Color c;
        if (m_Rainbow)
            c = Color::FromHSV(s.hue, 1.0f, 1.0f, alpha);
        else
            c = {alpha, alpha, alpha, 255};

        Shapes::DrawCircle(sx, sy, size, c, OriginMode::Center);
    }
}
