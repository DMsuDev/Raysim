#include "Raysim/Raysim.hpp"
#include "Raysim/Core/EntryPoint.hpp"

#include <cmath>
#include <vector>
#include <algorithm>
#include <string>
#include <sstream>
#include <iomanip>

using namespace RS;

/// Format a float to a fixed number of decimal places.
static std::string FormatFloat(float value, int decimals)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << value;
    return oss.str();
}

struct LissajousPreset {
    std::string name;
    float frequencyX;
    float frequencyY;
    std::string description;
};

static const std::vector<LissajousPreset> PRESETS = {
    {
        "Diagonal Line",
        1.0f, 1.0f,
        "Ratio 1:1 -> oscillators are synchronized"
    },
    {
        "Figure Eight",
        2.0f, 1.0f,
        "Ratio 2:1 -> classic oscilloscope pattern"
    },
    {
        "Parabola",
        1.0f, 2.0f,
        "Ratio 1:2 -> inverted version of the above"
    },
    {
        "3-Petal Rose",
        3.0f, 2.0f,
        "Ratio 3:2 -> symmetric floral pattern"
    },
    {
        "4-Petal Rose",
        3.0f, 1.0f,
        "Ratio 3:1 -> more complex symmetric shape"
    },
    {
        "Complex Pattern",
        5.0f, 4.0f,
        "Ratio 5:4 -> intricate combination"
    }
};

class LissajousSimulation : public Scene {
    RS_SCENE(LissajousSimulation)

private:
#pragma region Parameters and state variables
    /// Oscillation frequency on the X axis (higher = more cycles)
    float frequencyX = 3.0f;

    /// Oscillation frequency on the Y axis
    /// The ratio fx:fy determines the curve shape
    float frequencyY = 2.0f;

    /// Phase shift (rotation) in radians [0, 2*pi]
    float phaseShift = 0.0f;
    /// Rotation speed for the phase (radians per second), configurable via constructor
    float phaseRotationSpeed = 0.2f; // increased default speed
    bool animatePhase = false;

    std::vector<Math::Vec2f> curvePoints;
    int resolution = 1200;
    std::vector<float> paramT;

    //--- Smoothing frequencies --------
    float smoothFrequencyX = 3.0f;
    float smoothFrequencyY = 2.0f;
    float freqSmoothSpeed = 8.0f;

    //--- FIXED TIMESTEP (maintain visual consistency) ----
    const float angularSpeed = 2.0f;

    //--- Moving dot state (for alpha interpolation in Draw) --------------
    float dotAngle_     = 0.0f;
    float prevDotAngle_ = 0.0f;

    //--- COLOR PALETTE (interpolated along the curve) ----
    std::vector<Color> colors = {
        Color{255, 100, 150},   // Pink
        Color{100, 200, 255},   // Light Blue
        Color{150, 255, 100},   // Light Green
        Color{255, 200, 100},   // Orange
        Color{200, 100, 255},   // Purple
        Color{255, 255, 100},   // Yellow
    };

    //--- SCREEN LAYOUT (computed from window size) -------
    float centerX = 600.0f;
    float centerY = 400.0f;
    float curveRadius = 300.0f;
    float rectX = 300.0f;
    float rectY = 100.0f;
    float rectW = 600.0f;
    float rectH = 600.0f;

    int currentPreset = 3;

#pragma endregion

//==============================================================================
// Helper methods for curve generation and drawing
//==============================================================================

#pragma region Utilities

    /// Update the phase using the framework's fixed timestep
    void UpdatePhase(float fixedDt) {
        phaseShift += phaseRotationSpeed * fixedDt;
    }

    /// Smooth frequency transitions using exponential interpolation
    void SmoothFrequencies(float dt) {
        float alpha = 1.0f - std::exp(-freqSmoothSpeed * dt);
        smoothFrequencyX = Math::Lerp(smoothFrequencyX, frequencyX, alpha);
        smoothFrequencyY = Math::Lerp(smoothFrequencyY, frequencyY, alpha);
    }

    /// Draw the curve with color interpolation along palette
    void DrawCurveLines() {
        const int paletteSize = static_cast<int>(colors.size());
        size_t n = curvePoints.size();
        if (n < 2) return;
        for (size_t i = 0; i + 1 < n; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(n - 1);
            float palettePos = t * (paletteSize - 1);
            int idx = static_cast<int>(std::floor(palettePos));
            Color c = Color::Lerp(colors[idx],
                                  colors[std::min(idx + 1, paletteSize - 1)],
                                  palettePos - idx);
            Shapes::DrawLine(curvePoints[i].x, curvePoints[i].y,
                             curvePoints[i + 1].x, curvePoints[i + 1].y, c, 1.0f);
        }
    }

    /// Draw markers at regular intervals along the curve
    void DrawMarkers() {
        const int paletteSize = static_cast<int>(colors.size());
        size_t n = curvePoints.size();
        for (size_t i = 0; i < n; i += 60) {
            float t = static_cast<float>(i) / static_cast<float>(n - 1);
            float palettePos = t * (paletteSize - 1);
            int idx = static_cast<int>(std::floor(palettePos));
            Color markerColor = Color::Lerp(colors[idx],
                                            colors[std::min(idx + 1, paletteSize - 1)],
                                            palettePos - idx);
            Shapes::DrawCircle(curvePoints[i].x, curvePoints[i].y, 3.0f,
                               markerColor, OriginMode::Center);
        }
    }

    /// Draw axes and reference rectangle
    void DrawAxes() {
        Color axisColor = Colors::LightGray;
        Shapes::DrawRectOutline(rectX, rectY, rectW, rectH, axisColor);
        Shapes::DrawLine(centerX, rectY, centerX, rectY + rectH, axisColor);
        Shapes::DrawLine(rectX, centerY, rectX + rectW, centerY, axisColor);
    }

    /// Draw a moving point along the curve with constant angular velocity.
    /// Uses alpha to interpolate the dot between last and current angle snapshot,
    /// keeping sub-frame motion smooth regardless of frame rate.
    void DrawCurrentPoint(float alpha) {
        if (curvePoints.empty()) return;
        float angle  = Math::Lerp(prevDotAngle_, dotAngle_, alpha);
        float t      = std::fmod(angle, Math::TWO_PI) / Math::TWO_PI;
        float idxF   = t * static_cast<float>(curvePoints.size() - 1);
        int   idx    = static_cast<int>(std::floor(idxF));
        int   next   = std::min(idx + 1, static_cast<int>(curvePoints.size()) - 1);
        Math::Vec2f pos  = Math::Lerp(curvePoints[idx], curvePoints[next], idxF - idx);
        Shapes::DrawCircle(pos.x, pos.y, 10, Colors::RayYellow, OriginMode::Center);
    }

    /// Draw the user interface with parameter information
    /// Shows: frequencies, phase, resolution, FPS, current preset
    void DrawUI() {
        const int lineHeight = 30;
        float y = 30;

        // Frequency X
        Text::RenderText("Frequency X: " + FormatFloat(frequencyX, 2) + " (Left/Right)",
                         30, y, 20, Color{150, 255, 100}, OriginMode::TopLeft);
        y += lineHeight;

        // Frequency Y
        Text::RenderText("Frequency Y: " + FormatFloat(frequencyY, 2) + " (Up/Down)",
                         30, y, 20, Color{255, 200, 100}, OriginMode::TopLeft);
        y += lineHeight;

        // Phase shift and rotation speed
        float phaseDegrees = std::fmod(phaseShift * 180.0f / Math::PI, 360.0f);
        Text::RenderText("Phase: " + FormatFloat(phaseDegrees, 1) + "\xC2\xB0 | Speed: " +
                         FormatFloat(phaseRotationSpeed, 3) + " (P/O dir, +/- speed)",
                         30, y, 20, Color{100, 200, 255}, OriginMode::TopLeft);
        y += lineHeight;

        // Frequency ratio
        Text::RenderText("Ratio Fx:Fy = " + FormatFloat(frequencyX, 2) + ":" + FormatFloat(frequencyY, 2),
                         30, y, 20, Color{255, 150, 200}, OriginMode::TopLeft);
        y += lineHeight;

        // Current preset
        Text::RenderText("Preset [" + std::to_string(currentPreset + 1) + "/6]: " +
                         PRESETS[currentPreset].name,
                         30, y, 20, Color{200, 100, 255}, OriginMode::TopLeft);
        y += lineHeight;

        // Resolution and FPS
        Text::RenderText("Res: " + std::to_string(resolution) + " | FPS: " +
                         std::to_string(Time::GetFPS()),
                         30, y, 20, Color{200, 255, 100}, OriginMode::TopLeft);

        // Controls at the bottom
        float bottomY = static_cast<float>(GetWindow().GetHeight()) - 50.0f;
        Text::RenderText("Arrows=Freq  P/O=Dir  +/-=Speed  1-6=Preset  R=Reset",
                   20, bottomY, 18, Colors::LightGray, OriginMode::BottomLeft);

        // Description of the current preset
        bottomY += 25;
        Text::RenderText("Info: " + PRESETS[currentPreset].description,
                         20, bottomY, 16, Color{200, 200, 150}, OriginMode::BottomLeft);
    }

    /// Generate curve points using parametric Lissajous equations
    void GenerateCurve() {
        if (static_cast<int>(curvePoints.size()) != resolution)
            curvePoints.resize(resolution);
        for (int i = 0; i < resolution; ++i) {
            float t = paramT[i];
            float x = curveRadius * std::sin(smoothFrequencyX * t + phaseShift);
            float y = curveRadius * std::sin(smoothFrequencyY * t);
            curvePoints[i] = Math::Vec2f(centerX + x, centerY + y);
        }
    }

#pragma endregion

public:

//=============================================================================
// Application lifecycle methods
//=============================================================================

#pragma region Setup

    void OnStart() override {
        RS_DEBUG("LissajousSimulation starting: resolution={}, preset={} ({})",
                resolution, currentPreset + 1, PRESETS[currentPreset].name);

        RS_ASSERT(resolution > 0, "Curve resolution must be > 0");

        GetWindow().SetSize(1200, 800);
        GetWindow().SetTitle("Raysim - Lissajous Curves");
        FontManager::LoadFont("opensans", "fonts/OpenSans-Regular.ttf", 32);
        Time::SetTargetFPS(60);

        curvePoints.reserve(resolution);
        paramT.resize(resolution);
        for (int i = 0; i < resolution; ++i) {
            paramT[i] = static_cast<float>(i) / resolution * Math::TWO_PI;
        }

        smoothFrequencyX = frequencyX;
        smoothFrequencyY = frequencyY;
        GenerateCurve();
    }

#pragma endregion

#pragma region Update and Draw

    void OnUpdate(float dt) override {
        // Recompute layout from current window size
        float w = static_cast<float>(GetWindow().GetWidth());
        float h = static_cast<float>(GetWindow().GetHeight());
        centerX     = w * 0.5f;
        centerY     = h * 0.5f;
        curveRadius = Math::MinValue(w, h) * 0.375f;
        rectW       = curveRadius * 2.0f;
        rectH       = curveRadius * 2.0f;
        rectX       = centerX - curveRadius;
        rectY       = centerY - curveRadius;

        // INTERACTIVE CONTROLS
        // Frequency control (arrow keys) 1.2 units/sec
        if (GetInput().IsKeyDown(KeyCode::Right)) frequencyX += 1.2f * dt;
        if (GetInput().IsKeyDown(KeyCode::Left)) frequencyX = std::max(0.1f, frequencyX - 1.2f * dt);
        if (GetInput().IsKeyDown(KeyCode::Up)) frequencyY += 1.2f * dt;
        if (GetInput().IsKeyDown(KeyCode::Down)) frequencyY = std::max(0.1f, frequencyY - 1.2f * dt);

        // Phase control: P/O for direction, +/- for speed adjustment 0.3 units/sec
        if (GetInput().IsKeyDown(KeyCode::P)) phaseRotationSpeed = std::abs(phaseRotationSpeed);
        if (GetInput().IsKeyDown(KeyCode::O)) phaseRotationSpeed = -std::abs(phaseRotationSpeed);
        if (GetInput().IsKeyDown(KeyCode::Equal) || GetInput().IsKeyDown(KeyCode::KPAdd))
            phaseRotationSpeed += 0.3f * dt;
        if (GetInput().IsKeyDown(KeyCode::Minus) || GetInput().IsKeyDown(KeyCode::KPSubtract))
            phaseRotationSpeed -= 0.3f * dt;

        // Select presets with number keys 1-6
        for (int i = 0; i < 6; ++i) {
            if (GetInput().IsKeyPressed(static_cast<KeyCode>(static_cast<int>(KeyCode::D1) + i))) {
                currentPreset = i;
                frequencyX = PRESETS[i].frequencyX;
                frequencyY = PRESETS[i].frequencyY;
                phaseShift = 0.0f;
            }
        }

        if (GetInput().IsKeyPressed(KeyCode::R)) {
            frequencyX = 3.0f;
            frequencyY = 2.0f;
            phaseShift = 0.0f;
            currentPreset = 3;
        }

        SmoothFrequencies(dt);
        GenerateCurve();

        // Advance dot angle - snapshot prev first so Draw(alpha) can interpolate
        prevDotAngle_ = dotAngle_;
        dotAngle_ += angularSpeed * dt;
    }

    void OnFixedUpdate(float fixedDt) override {
        UpdatePhase(fixedDt);
    }

    void OnDraw(float alpha) override {
        GetRenderer().ClearScreen(Colors::RayBlack);

        DrawCurveLines();
        DrawMarkers();
        DrawAxes();
        DrawCurrentPoint(alpha);
        DrawUI();
    }

#pragma endregion

};


//==============================================================================
// Entry point
//==============================================================================

RS::Application* RS::CreateApplication(RS::ApplicationCommandLineArgs args)
{
    RS::ApplicationConfig config;
    config.Window.Title  = "Raysim - Lissajous Curves Demo";
    config.Window.Width  = 1000;
    config.Window.Height = 600;

    auto* app = new RS::Application(config);
    app->RegisterScene<LissajousSimulation>();
    app->ChangeScene<LissajousSimulation>();
    return app;
}
