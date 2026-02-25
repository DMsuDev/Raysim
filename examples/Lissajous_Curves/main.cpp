// ─────────────────────────────────────────────────────────────────────────────
//  RaySim - Lissajous Curves: Interactive Visualization
//
//  DESCRIPTION:
//  This example demonstrates an interactive visualization of Lissajous curves,
//  mathematical figures produced by two perpendicular oscillators with
//  different frequencies.
//
//  PARAMETRIC EQUATIONS:
//    x(t) = A·sin(nx·t + φ)
//    y(t) = A·sin(ny·t)
//
//  Where:
//    - nx, ny: oscillation frequencies on X and Y
//    - φ: phase shift between oscillators
//    - A: amplitude (curve radius)
//    - t: time parameter [0, 2π]
//
//  REFERENCES:
//  - Lissajous curves: https://en.wikipedia.org/wiki/Lissajous_curve
//  - Simple harmonic motion: https://en.wikipedia.org/wiki/Simple_harmonic_motion
//
//  INTERACTIVE CONTROLS:
//  - Left/Right Arrows: change X frequency (nx)
//  - Up/Down Arrows: change Y frequency (ny)
//  - P: rotate phase forward
//  - O: rotate phase backward
//  - 1-6: select famous presets
//  - R: reset defaults
// ─────────────────────────────────────────────────────────────────────────────

#include "Raysim/Raysim.hpp"

#include <cmath>
#include <vector>
#include <cstdio>
#include <algorithm>
#include <string>

using namespace RS;

// ─────────────────────────────────────────────────────────────────────────────
//  FAMOUS LISSAJOUS PRESETS
//
//  These presets show known frequency combinations that produce
//  interesting and symmetric patterns.
// ─────────────────────────────────────────────────────────────────────────────

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
        "Ratio 1:1 → oscillators are synchronized"
    },
    {
        "Figure Eight",
        2.0f, 1.0f,
        "Ratio 2:1 → classic oscilloscope pattern"
    },
    {
        "Parabola",
        1.0f, 2.0f,
        "Ratio 1:2 → inverted version of the above"
    },
    {
        "3-Petal Rose",
        3.0f, 2.0f,
        "Ratio 3:2 → symmetric floral pattern"
    },
    {
        "4-Petal Rose",
        3.0f, 1.0f,
        "Ratio 3:1 → more complex symmetric shape"
    },
    {
        "Complex Pattern",
        5.0f, 4.0f,
        "Ratio 5:4 → intricate combination"
    }
};

class LissajousSimulation : public Canvas {
private:
    // ─── PARÁMETROS MATEMÁTICOS PRINCIPALES ─────────────────────────────────
    /// Oscillation frequency on the X axis (higher = more cycles)
    float frequencyX = 3.0f;

    /// Oscillation frequency on the Y axis
    /// The ratio fx:fy determines the curve shape
    float frequencyY = 2.0f;

    /// Phase shift (rotation) in radians [0, 2π]
    float phaseShift = 0.0f;
    /// Rotation speed for the phase (radians per second), configurable via constructor
    float phaseRotationSpeed = 0.2f; // increased default speed
    bool animatePhase = false;

    // ─── POINTS AND PARAMETRIZATION
    std::vector<Vector2> curvePoints;
    int resolution = 1200;
    std::vector<float> paramT;

    // ─── FREQUENCY SMOOTHING (avoids abrupt visual changes)
    float smoothFrequencyX = 3.0f;
    float smoothFrequencyY = 2.0f;
    float freqSmoothSpeed = 8.0f;

    // ─── FIXED TIMESTEP (maintain visual consistency)
    float accumulator = 0.0f;
    const float fixedDt = 1.0f / 120.0f;
    const float angularSpeed = 2.0f;

    // ─── COLOR PALETTE (interpolated along the curve)
    std::vector<Color> colors = {
        Color{255, 100, 150},   // Pink
        Color{100, 200, 255},   // Light Blue
        Color{150, 255, 100},   // Light Green
        Color{255, 200, 100},   // Orange
        Color{200, 100, 255},   // Purple
        Color{255, 255, 100},   // Yellow
    };

    // ─── SCREEN LAYOUT
    const float centerX = 600.0f;
    const float centerY = 400.0f;
    const float curveRadius = 300.0f;
    const float rectX = 300.0f;
    const float rectY = 100.0f;
    const float rectW = 600.0f;
    const float rectH = 600.0f;

    int currentPreset = 3;

    // ─── HELPER METHODS (simplify the main logic) ──────────────────────

    /// Update the phase using a fixed timestep
    /// Keeps animation smooth even if the framerate varies
    /// FORMULA: phaseShift(t+dt) = phaseShift(t) + phaseRotationSpeed · dt
    void UpdatePhase(float dt) {
        accumulator += dt;
        while (accumulator >= fixedDt) {
            // Use the phase rotation speed (configurable via P/O keys)
            phaseShift += phaseRotationSpeed * fixedDt;
            accumulator -= fixedDt;
        }
    }

    /// Smooth frequency transitions using exponential interpolation
    void SmoothFrequencies(float dt) {
        float alpha = 1.0f - std::exp(-freqSmoothSpeed * dt);
        smoothFrequencyX = Math::Lerp(smoothFrequencyX, frequencyX, alpha);
        smoothFrequencyY = Math::Lerp(smoothFrequencyY, frequencyY, alpha);
    }

    /// Draw the curve with color interpolation along palette
    void DrawCurveLines() {
        const int paletteSize = (int)colors.size();
        size_t n = curvePoints.size();
        if (n < 2) return;
        for (size_t i = 0; i + 1 < n; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(n - 1);
            float palettePos = t * (paletteSize - 1);
            int idx = (int)std::floor(palettePos);
            Color c = Color::Lerp(colors[idx],
                                  colors[std::min(idx + 1, paletteSize - 1)],
                                  palettePos - idx);
            Shapes::DrawLine(curvePoints[i].x, curvePoints[i].y,
                             curvePoints[i + 1].x, curvePoints[i + 1].y, c, 1.0f);
        }
    }

    /// Draw markers at regular intervals along the curve
    void DrawMarkers() {
        const int paletteSize = (int)colors.size();
        size_t n = curvePoints.size();
        for (size_t i = 0; i < n; i += 60) {
            float t = static_cast<float>(i) / static_cast<float>(n - 1);
            float palettePos = t * (paletteSize - 1);
            int idx = (int)std::floor(palettePos);
            Color markerColor = Color::Lerp(colors[idx],
                                            colors[std::min(idx + 1, paletteSize - 1)],
                                            palettePos - idx);
            Shapes::DrawCircle(curvePoints[i].x, curvePoints[i].y, 3.0f,
                               markerColor, OriginMode::Center);
        }
    }

    /// Draw axes and reference rectangle
    void DrawAxes() {
        Color axisColor = Color::LightGray();
        Shapes::DrawRectOutline(rectX, rectY, rectW, rectH, axisColor);
        Shapes::DrawLine(centerX, rectY, centerX, rectY + rectH, axisColor);
        Shapes::DrawLine(rectX, centerY, rectX + rectW, centerY, axisColor);
    }

    /// Draw a moving point along the curve with constant angular velocity
    void DrawCurrentPoint(const TimeManager& time) {
        if (curvePoints.empty()) return;
        float currentT = std::fmod(time.GetElapsedTime() * angularSpeed, Math::TWO_PI) / Math::TWO_PI;
        float idxF = currentT * (static_cast<float>(curvePoints.size() - 1));
        int idx = static_cast<int>(std::floor(idxF));
        int next = std::min(idx + 1, (int)curvePoints.size() - 1);
        Vector2 pos = Math::Lerp(curvePoints[idx], curvePoints[next], idxF - idx);
        Shapes::DrawCircle(pos.x, pos.y, 10, Color::RayYellow(), OriginMode::Center);
    }

    /// Draw the user interface with parameter information
    /// Shows: frequencies, phase, resolution, FPS, current preset
    void DrawUI(const TimeManager& time) {
        const int lineHeight = 30;
        int y = 30;
        char buf[256];

        // Frequency X
        std::snprintf(buf, sizeof(buf), "Frequency X: %.2f (Left/Right)", frequencyX);
        Text::DrawText(std::string(buf), 30, y, 20, Color{150, 255, 100},
                   OriginMode::TopLeft);
        y += lineHeight;

        // Frequency Y
        std::snprintf(buf, sizeof(buf), "Frequency Y: %.2f (Up/Down)", frequencyY);
        Text::DrawText(std::string(buf), 30, y, 20, Color{255, 200, 100},
                   OriginMode::TopLeft);
        y += lineHeight;

        // Phase shift and rotation speed
        float phaseDegrees = std::fmod(phaseShift * 180.0f / Math::PI, 360.0f);
        std::snprintf(buf, sizeof(buf), "Phase: %.1f° | Speed: %.3f (P/O dir, +/- speed)",
                      phaseDegrees, phaseRotationSpeed);
        Text::DrawText(std::string(buf), 30, y, 20, Color{100, 200, 255},
                   OriginMode::TopLeft);
        y += lineHeight;

        // Frequency ratio (key to understanding the shape)
        std::snprintf(buf, sizeof(buf), "Ratio Fx:Fy = %.2f:%.2f",
                  frequencyX, frequencyY);
        Text::DrawText(std::string(buf), 30, y, 20, Color{255, 150, 200},
                   OriginMode::TopLeft);
        y += lineHeight;

        // Current preset
        std::snprintf(buf, sizeof(buf), "Preset [%d/6]: %s", currentPreset + 1,
                      PRESETS[currentPreset].name.c_str());
        Text::DrawText(std::string(buf), 30, y, 20, Color{200, 100, 255},
                       OriginMode::TopLeft);
        y += lineHeight;

        // Resolution and FPS
        std::snprintf(buf, sizeof(buf), "Res: %d | FPS: %d", resolution,
                      time.GetCurrentFPS());
        Text::DrawText(std::string(buf), 30, y, 20, Color{200, 255, 100},
                       OriginMode::TopLeft);

        // Controls at the bottom
        y = 750;
        Text::DrawText("Arrows=Freq  P/O=Dir  +/-=Speed  1-6=Preset  R=Reset",
                   20, y, 18, Color::LightGray(), OriginMode::BottomLeft);

        // Description of the current preset
        y += 25;
        std::snprintf(buf, sizeof(buf), "Info: %s",
                      PRESETS[currentPreset].description.c_str());
        Text::DrawText(std::string(buf), 20, y, 16, Color{200, 200, 150},
                       OriginMode::BottomLeft);
    }

    /// Generate curve points using parametric Lissajous equations
    void GenerateCurve() {
        if ((int)curvePoints.size() != resolution)
            curvePoints.resize(resolution);
        for (int i = 0; i < resolution; ++i) {
            float t = paramT[i];
            float x = curveRadius * std::sin(smoothFrequencyX * t + phaseShift);
            float y = curveRadius * std::sin(smoothFrequencyY * t);
            curvePoints[i] = Vector2(centerX + x, centerY + y);
        }
    }

public:
    LissajousSimulation(float initialPhaseSpeed = 0.2f) {
        phaseRotationSpeed = initialPhaseSpeed;
    }

    /// Initialize the simulation and precompute parameter values
    void Setup() override {
        SetSize(1200, 800);
        SetTitle("RaySim - Lissajous Curves");
        SetDefaultFont("resources/fonts/opensans/OpenSans-Regular.ttf", 64);
        SetTargetFPS(60);

        curvePoints.reserve(resolution);
        paramT.resize(resolution);
        for (int i = 0; i < resolution; ++i) {
            paramT[i] = (float)i / resolution * Math::TWO_PI;
        }

        smoothFrequencyX = frequencyX;
        smoothFrequencyY = frequencyY;
        GenerateCurve();
    }

    void Draw() override {
        Background(Color::RayBlack());

        InputManager& input = GetInputManager();
        TimeManager& time = GetTimeManager();

        // ───────────────────────────────────────────────────────────────────────
        // INTERACTIVE CONTROLS
        // ───────────────────────────────────────────────────────────────────────

        // Frequency control (arrow keys)
        if (input.IsKeyDown(KeyCode::Right)) frequencyX += 0.02f;
        if (input.IsKeyDown(KeyCode::Left)) frequencyX = std::max(0.1f, frequencyX - 0.02f);
        if (input.IsKeyDown(KeyCode::Up)) frequencyY += 0.02f;
        if (input.IsKeyDown(KeyCode::Down)) frequencyY = std::max(0.1f, frequencyY - 0.02f);

        // Phase control: P/O for direction, +/- for speed adjustment
        if (input.IsKeyDown(KeyCode::P)) phaseRotationSpeed = std::abs(phaseRotationSpeed);
        if (input.IsKeyDown(KeyCode::O)) phaseRotationSpeed = -std::abs(phaseRotationSpeed);
        if (input.IsKeyDown(KeyCode::Equal) || input.IsKeyDown(KeyCode::KP_Add))
            phaseRotationSpeed += 0.005f;
        if (input.IsKeyDown(KeyCode::Minus) || input.IsKeyDown(KeyCode::KP_Subtract))
            phaseRotationSpeed -= 0.005f;

        // Select presets with number keys 1-6
        for (int i = 0; i < 6; ++i) {
            if (input.IsKeyPressed((KeyCode)((int)KeyCode::One + i))) {
                currentPreset = i;
                frequencyX = PRESETS[i].frequencyX;
                frequencyY = PRESETS[i].frequencyY;
                phaseShift = 0.0f;
            }
        }

        if (input.IsKeyPressed(KeyCode::R)) {
            frequencyX = 3.0f;
            frequencyY = 2.0f;
            phaseShift = 0.0f;
            currentPreset = 3;
        }

        float dt = time.GetDeltaTime();

        UpdatePhase(dt);
        SmoothFrequencies(dt);
        GenerateCurve();

        DrawCurveLines();
        DrawMarkers();
        DrawAxes();
        DrawCurrentPoint(time);
        DrawUI(time);
    }
};

int main() {
    LissajousSimulation sim;
    sim.Run();
    return 0;
}
