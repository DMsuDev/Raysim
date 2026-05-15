#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

#include <functional>
#include <vector>

using namespace RS;

// Forward declarations
class StarField;
class PlasmaArt;
class ClockMandala;

// ============================================================================
// SceneNavLayer
// Floating top-center overlay shared by all scenes.
// ============================================================================

class SceneNavLayer : public Layer
{
public:
    SceneNavLayer(const char* sceneName, int index, int total,
                  std::function<void()> onNext,
                  std::function<void()> onPrev)
        : Layer("SceneNavLayer")
        , m_Name(sceneName), m_Index(index), m_Total(total)
        , m_OnNext(std::move(onNext)), m_OnPrev(std::move(onPrev))
    {}

    void OnUIRender() override
    {
        ImGuiIO& io = ImGui::GetIO();
        ImGui::SetNextWindowPos(ImVec2(io.DisplaySize.x * 0.5f, 10.0f),
                                ImGuiCond_Always, ImVec2(0.5f, 0.0f));
        ImGui::SetNextWindowBgAlpha(0.72f);
        ImGuiWindowFlags flags =
            ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_AlwaysAutoResize |
            ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoFocusOnAppearing |
            ImGuiWindowFlags_NoNav | ImGuiWindowFlags_NoMove;

        ImGui::Begin("##scenenav", nullptr, flags);

        ImGui::PushStyleColor(ImGuiCol_Button,        ImVec4(0.15f, 0.15f, 0.38f, 1.0f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.30f, 0.30f, 0.65f, 1.0f));

        if (ImGui::Button(" << ")) m_OnPrev();
        ImGui::SameLine();
        ImGui::Text("  %s  [%d / %d]  ", m_Name, m_Index, m_Total);
        ImGui::SameLine();
        if (ImGui::Button(" >> ")) m_OnNext();

        ImGui::PopStyleColor(2);
        ImGui::End();
    }

private:
    const char* m_Name;
    int m_Index, m_Total;
    std::function<void()> m_OnNext;
    std::function<void()> m_OnPrev;
};

// ============================================================================
// StarFieldControlLayer
// In-scene ImGui panel: warp speed + rainbow toggle.
// ============================================================================

class StarFieldControlLayer : public Layer
{
public:
    StarFieldControlLayer(float& speed, bool& rainbow)
        : Layer("StarFieldCtrl"), m_Speed(speed), m_Rainbow(rainbow) {}

    void OnUIRender() override
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
        ImGui::SliderFloat("Speed",      &m_Speed,   0.1f, 6.0f);
        ImGui::Checkbox("Rainbow stars", &m_Rainbow);
        ImGui::End();
    }

private:
    float& m_Speed;
    bool&  m_Rainbow;
};

// ============================================================================
// StarField Scene 1
// 300 stars in 3-D perspective rushing toward the viewer.
// ============================================================================

class StarField : public Scene
{
    RS_SCENE(StarField)

    struct Star { float x, y, z, hue; };

    static constexpr int   NUM_STARS  = 300;
    static constexpr float BASE_SPEED = 0.35f;

    std::vector<Star> m_Stars;
    float m_Speed   = 1.0f;
    bool  m_Rainbow = false;

    void Reset(Star& s, bool spread = true);

public:
    void OnStart()               override;
    void OnUpdate(float dt)      override;
    void OnDraw(float alpha) override;
};

// ============================================================================
// PlasmaControlLayer
// In-scene ImGui panel: frequency X/Y + animation speed.
// ============================================================================

class PlasmaControlLayer : public Layer
{
public:
    PlasmaControlLayer(float& fx, float& fy, float& speed)
        : Layer("PlasmaCtrl"), m_FX(fx), m_FY(fy), m_Speed(speed) {}

    void OnUIRender() override
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

private:
    float& m_FX;
    float& m_FY;
    float& m_Speed;
};

// ============================================================================
// PlasmaArt Scene 2
// CPU plasma: 50x35 color tiles driven by 4 layered sine waves.
// ============================================================================

class PlasmaArt : public Scene
{
    RS_SCENE(PlasmaArt)

    static constexpr int COLS = 50;
    static constexpr int ROWS = 35;

    float m_Time  = 0.0f;
    float m_FX    = 3.0f;
    float m_FY    = 2.5f;
    float m_Speed = 1.0f;

public:
    void OnStart()               override;
    void OnUpdate(float dt)      override;
    void OnDraw(float alpha) override;
};

// ============================================================================
// MandalaControlLayer
// In-scene ImGui panel: ring count + global rotation speed.
// ============================================================================

class MandalaControlLayer : public Layer
{
public:
    MandalaControlLayer(int& rings, float& speed)
        : Layer("MandalaCtrl"), m_Rings(rings), m_Speed(speed) {}

    void OnUIRender() override
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

private:
    int&   m_Rings;
    float& m_Speed;
};

// ============================================================================
// ClockMandala Scene 3
// Concentric rings of rotating polygons with pulsing starburst core.
// ============================================================================

class ClockMandala : public Scene
{
    RS_SCENE(ClockMandala)

    float m_Time  = 0.0f;
    int   m_Rings = 5;
    float m_Speed = 1.0f;

public:
    void OnStart()               override;
    void OnUpdate(float dt)      override;
    void OnDraw(float alpha) override;
};

// ============================================================================
// StarField - implementations
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
    window().SetTitle("SceneShowcase - StarField");

    m_Stars.resize(NUM_STARS);
    for (auto& s : m_Stars) Reset(s, true);

    SetupImGuiLayer();
    PushLayer(new StarFieldControlLayer(m_Speed, m_Rainbow));
    PushOverlay(new SceneNavLayer("StarField", 1, 3,
        [this]() { sceneManager().ChangeScene<PlasmaArt>(); },
        [this]() { sceneManager().ChangeScene<ClockMandala>(); }
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

void StarField::OnDraw(float alpha)
{
    renderer().ClearScreen(Color{5, 5, 18});

    float hw = window().GetWidth()  * 0.5f;
    float hh = window().GetHeight() * 0.5f;
    float sw = static_cast<float>(window().GetWidth());
    float sh = static_cast<float>(window().GetHeight());

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

// ============================================================================
// PlasmaArt - implementations
// ============================================================================

void PlasmaArt::OnStart()
{
    window().SetTitle("SceneShowcase - PlasmaArt");

    SetupImGuiLayer();
    PushLayer(new PlasmaControlLayer(m_FX, m_FY, m_Speed));
    PushOverlay(new SceneNavLayer("PlasmaArt", 2, 3,
        [this]() { sceneManager().ChangeScene<ClockMandala>(); },
        [this]() { sceneManager().ChangeScene<StarField>(); }
    ));
}

void PlasmaArt::OnUpdate(float dt)
{
    m_Time += dt * m_Speed;
}

void PlasmaArt::OnDraw(float alpha)
{
    renderer().ClearScreen(Colors::Black);

    float cw = static_cast<float>(window().GetWidth())  / COLS;
    float ch = static_cast<float>(window().GetHeight()) / ROWS;

    for (int row = 0; row < ROWS; ++row)
    {
        for (int col = 0; col < COLS; ++col)
        {
            float nx = static_cast<float>(col) / COLS;
            float ny = static_cast<float>(row) / ROWS;

            float v = Math::Sin(nx * m_FX  * Math::PI + m_Time)
                    + Math::Sin(ny * m_FY  * Math::PI + m_Time * 0.7f)
                    + Math::Sin((nx + ny)  * 3.0f     + m_Time * 1.3f)
                    + Math::Sin(Math::Sqrt(nx * nx + ny * ny) * 5.0f - m_Time);

            float hue = Math::Mod((v + 4.0f) * 45.0f, 360.0f);
            Color c   = Color::FromHSV(hue, 1.0f, 1.0f);

            Shapes::DrawRect(col * cw, row * ch, cw + 1.0f, ch + 1.0f, c);
        }
    }
}

// ============================================================================
// ClockMandala - implementations
// ============================================================================

void ClockMandala::OnStart()
{
    window().SetTitle("SceneShowcase - ClockMandala");

    SetupImGuiLayer();
    PushLayer(new MandalaControlLayer(m_Rings, m_Speed));
    PushOverlay(new SceneNavLayer("ClockMandala", 3, 3,
        [this]() { sceneManager().ChangeScene<StarField>(); },
        [this]() { sceneManager().ChangeScene<PlasmaArt>(); }
    ));
}

void ClockMandala::OnUpdate(float dt)
{
    m_Time += dt * m_Speed;
}

void ClockMandala::OnDraw(float alpha)
{
    renderer().ClearScreen(Color{10, 5, 30});

    float w  = static_cast<float>(window().GetWidth());
    float h  = static_cast<float>(window().GetHeight());
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
        float hue    = Math::Mod(t * 240.0f + m_Time * 25.0f, 360.0f);

        Color cBright = Color::FromHSV(hue, 0.95f, 1.0f, 220);
        Color cDim    = Color::FromHSV(hue, 0.60f, 0.55f, 100);

        Shapes::DrawCircleOutline(cx, cy, radius, cDim, OriginMode::Center);

        int   sides    = 3 + (ring % 4);
        float nodeSize = 6.0f + (1.0f - t) * 14.0f;
        for (int n = 0; n < nodes; ++n)
        {
            float angle  = rot + n * (Math::TWO_PI / static_cast<float>(nodes));
            float px     = cx + Math::Cos(angle) * radius;
            float py     = cy + Math::Sin(angle) * radius;
            float rotDeg = Math::ToDegrees(angle) + m_Time * 60.0f * dir;
            Shapes::DrawPolygon(px, py, sides, nodeSize * 0.5f, rotDeg, cBright);
        }
    }

    // Pulsing starburst at the centre
    float pulse = 28.0f + Math::Sin(m_Time * 3.0f) * 8.0f;
    for (int i = 0; i < 16; ++i)
    {
        float angle = m_Time * 0.6f + i * (Math::TWO_PI / 16.0f);
        float len   = pulse + Math::Sin(m_Time * 5.0f + i * 0.7f) * 6.0f;
        float ex    = cx + Math::Cos(angle) * len;
        float ey    = cy + Math::Sin(angle) * len;
        Color lc    = Color::FromHSV(Math::Mod(m_Time * 80.0f + i * 22.5f, 360.0f), 1.0f, 1.0f);
        Shapes::DrawLine(cx, cy, ex, ey, lc, 2.0f);
    }

    Shapes::DrawCircle(cx, cy, 6.0f, Colors::White, OriginMode::Center);
}

// ============================================================================
// Entry point
// ============================================================================

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs /*args*/)
{
    RS::ApplicationConfig config;
    config.Window.Title  = "SceneShowcase";
    config.Window.Width  = 1000;
    config.Window.Height = 700;
    config.TargetFPS     = 60;
    config.VSync         = true;

    auto* app = new RS::Application(config);
    app->RegisterScene<StarField>();
    app->RegisterScene<PlasmaArt>();
    app->RegisterScene<ClockMandala>();
    app->ChangeScene<StarField>();
    return app;
}
