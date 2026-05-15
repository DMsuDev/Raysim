#include "LissajousSimulation.hpp"

#include <iostream>
#include <sstream>
#include <iomanip>

namespace RS
{

static const std::vector<LissajousPreset> s_presets = {
    {"Diagonal Line",   1.0f, 1.0f, "Ratio 1:1 -> oscillators are synchronized"},
    {"Figure Eight",    2.0f, 1.0f, "Ratio 2:1 -> classic oscilloscope pattern"},
    {"Parabola",        1.0f, 2.0f, "Ratio 1:2 -> inverted version of the above"},
    {"3-Petal Rose",    3.0f, 2.0f, "Ratio 3:2 -> symmetric floral pattern"},
    {"4-Petal Rose",    3.0f, 1.0f, "Ratio 3:1 -> more complex symmetric shape"},
    {"Complex Pattern", 5.0f, 4.0f, "Ratio 5:4 -> intricate combination"}
};

static const ColorPalette s_defaultPalette = {
    {
        Color{255, 100, 150},
        Color{100, 200, 255},
        Color{150, 255, 100},
        Color{255, 200, 100},
        Color{200, 100, 255},
        Color{255, 255, 100}
    }
};

const std::vector<LissajousPreset>& LissajousPresets::GetAll() { return s_presets; }
const LissajousPreset& LissajousPresets::Get(size_t index) { return s_presets[index % s_presets.size()]; }
size_t LissajousPresets::Count() { return s_presets.size(); }

int LissajousPresets::FindPreset(float freqX, float freqY)
{
    for (size_t i = 0; i < s_presets.size(); ++i) {
        if (Math::Abs(s_presets[i].frequencyX - freqX) < 0.01f &&
            Math::Abs(s_presets[i].frequencyY - freqY) < 0.01f) {
            return static_cast<int>(i);
        }
    }
    return -1;
}

int GreatestCommonDivisor(int a, int b)
{
    a = Math::Abs(a);
    b = Math::Abs(b);
    while (b != 0) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

std::string LissajousPresets::GetRatioString(float freqX, float freqY)
{
    int ix = static_cast<int>(freqX * 100);
    int iy = static_cast<int>(freqY * 100);
    int g = GreatestCommonDivisor(ix, iy);
    if (g > 0) {
        return std::to_string(ix / g) + ":" + std::to_string(iy / g);
    }
    return std::to_string(freqX) + ":" + std::to_string(freqY);
}

const ColorPalette& ColorPalettes::GetDefault() { return s_defaultPalette; }
const std::vector<Color>& ColorPalettes::GetColors() { return s_defaultPalette.colors; }
int ColorPalettes::GetSize() { return static_cast<int>(s_defaultPalette.colors.size()); }

Color ColorPalettes::GetColorAt(float t, int paletteSize)
{
    const auto& colors = s_defaultPalette.colors;
    float palettePos = t * (paletteSize - 1);
    int idx = Math::FloorToInt(palettePos);
    idx = Math::Clamp(idx, 0, paletteSize - 1);
    int nextIdx = Math::MinValue(idx + 1, paletteSize - 1);
    return Color::Lerp(colors[idx], colors[nextIdx], palettePos - idx);
}

static std::string FormatFloat(float value, int decimals)
{
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(decimals) << value;
    return oss.str();
}

void LissajousSimulation::OnStart()
{
    const auto& preset = LissajousPresets::Get(currentPreset);
    RS_ASSERT(resolution > 0, "Curve resolution must be > 0");

    FontManager::LoadFont("opensans", "fonts/OpenSans-Regular.ttf", 32);

    curvePoints.resize(resolution);
    paramT.resize(resolution);
    for (int i = 0; i < resolution; ++i) {
        paramT[i] = static_cast<float>(i) / resolution * Math::TWO_PI;
    }

    smoothFrequencyX = frequencyX;
    smoothFrequencyY = frequencyY;
    cachedFreqX = frequencyX;
    cachedFreqY = frequencyY;
    cachedPhaseShift = phaseShift;
    curveDirty = true;
    GenerateCurve();
}

void LissajousSimulation::OnUpdate(float dt)
{
    float w = static_cast<float>(window().GetWidth());
    float h = static_cast<float>(window().GetHeight());
    centerX     = w * 0.5f;
    centerY     = h * 0.5f;
    curveRadius = Math::MinValue(w, h) * 0.40f;
    rectW       = curveRadius * 2.0f;
    rectH       = curveRadius * 2.0f;
    rectX       = centerX - curveRadius;
    rectY       = centerY - curveRadius;

    bool needsRegenerate = false;

    if (input().IsKeyDown(KeyCode::Right)) {
        frequencyX += 1.2f * dt;
        needsRegenerate = true;
    }
    if (input().IsKeyDown(KeyCode::Left)) {
        frequencyX = Math::MaxValue(0.1f, frequencyX - 1.2f * dt);
        needsRegenerate = true;
    }
    if (input().IsKeyDown(KeyCode::Up)) {
        frequencyY += 1.2f * dt;
        needsRegenerate = true;
    }
    if (input().IsKeyDown(KeyCode::Down)) {
        frequencyY = Math::MaxValue(0.1f, frequencyY - 1.2f * dt);
        needsRegenerate = true;
    }

    if (input().IsKeyDown(KeyCode::P)) phaseRotationSpeed = Math::Abs(phaseRotationSpeed);
    if (input().IsKeyDown(KeyCode::O)) phaseRotationSpeed = -Math::Abs(phaseRotationSpeed);
    if (input().IsKeyDown(KeyCode::Equal) || input().IsKeyDown(KeyCode::KPAdd)) {
        phaseRotationSpeed += 0.3f * dt;
        needsRegenerate = true;
    }
    if (input().IsKeyDown(KeyCode::Minus) || input().IsKeyDown(KeyCode::KPSubtract)) {
        phaseRotationSpeed -= 0.3f * dt;
        needsRegenerate = true;
    }

    for (size_t i = 0; i < LissajousPresets::Count(); ++i) {
        if (input().IsKeyPressed(static_cast<KeyCode>(static_cast<int>(KeyCode::D1) + i))) {
            currentPreset = static_cast<int>(i);
            const auto& preset = LissajousPresets::Get(i);
            frequencyX = preset.frequencyX;
            frequencyY = preset.frequencyY;
            phaseShift = 0.0f;
            needsRegenerate = true;
        }
    }

    if (input().IsKeyPressed(KeyCode::R)) {
        const auto& defaultPreset = LissajousPresets::Get(3);
        frequencyX = defaultPreset.frequencyX;
        frequencyY = defaultPreset.frequencyY;
        phaseShift = 0.0f;
        phaseRotationSpeed = 0.2f;
        currentPreset = 3;
        needsRegenerate = true;
    }

    SmoothFrequencies(dt);

    curveDirty |= (cachedFreqX != smoothFrequencyX || cachedFreqY != smoothFrequencyY || cachedPhaseShift != phaseShift);
    if (curveDirty) {
        cachedFreqX = smoothFrequencyX;
        cachedFreqY = smoothFrequencyY;
        cachedPhaseShift = phaseShift;
        curveDirty = false;
        GenerateCurve();
    }

    prevPointAngle = pointAngle;
    pointAngle += markerSpeed * dt;
}

void LissajousSimulation::OnFixedUpdate(float fixedDt)
{
    UpdatePhase(fixedDt);
}

void LissajousSimulation::OnDraw(float alpha)
{
    renderer().ClearScreen(Colors::RayBlack);

    DrawCurveTrails();
    DrawCurveLines();
    DrawMarkers();
    DrawAxes();
    DrawCurrentPoint(alpha);
    DrawUI();
}

void LissajousSimulation::UpdatePhase(float fixedDt)
{
    phaseShift += phaseRotationSpeed * fixedDt;
}

void LissajousSimulation::SmoothFrequencies(float dt)
{
    float alpha = 1.0f - Math::Exp(-freqSmoothSpeed * dt);
    smoothFrequencyX = Math::Lerp(smoothFrequencyX, frequencyX, alpha);
    smoothFrequencyY = Math::Lerp(smoothFrequencyY, frequencyY, alpha);
}

void LissajousSimulation::GenerateCurve()
{
    if (static_cast<int>(curvePoints.size()) != resolution)
        curvePoints.resize(resolution);
    for (int i = 0; i < resolution; ++i) {
        float t = paramT[i];
        float x = curveRadius * Math::Sin(smoothFrequencyX * t + phaseShift);
        float y = curveRadius * Math::Sin(smoothFrequencyY * t);
        curvePoints[i] = Math::Vec2f(centerX + x, centerY + y);
    }
}

void LissajousSimulation::DrawCurveTrails()
{
    const int paletteSize = ColorPalettes::GetSize();
    if (static_cast<int>(curvePoints.size()) < 2) return;

    struct Trail { float phaseOff; unsigned char alpha; };
    const Trail trails[] = { {-0.4f, 22} };

    for (const auto& tr : trails) {
        float phOff = phaseShift + tr.phaseOff;
        const int step = 3;
        const int n    = resolution - step;
        for (int i = 0; i < n; i += step) {
            float t0  = paramT[i];
            float t1  = paramT[i + step];
            float px  = centerX + curveRadius * Math::Sin(smoothFrequencyX * t0 + phOff);
            float py  = centerY + curveRadius * Math::Sin(smoothFrequencyY * t0);
            float px2 = centerX + curveRadius * Math::Sin(smoothFrequencyX * t1 + phOff);
            float py2 = centerY + curveRadius * Math::Sin(smoothFrequencyY * t1);
            float frac = static_cast<float>(i) / static_cast<float>(resolution - 1);
            Color c = ColorPalettes::GetColorAt(frac, paletteSize);
            c.a = tr.alpha;
            Shapes::DrawLine(px, py, px2, py2, c, 1.0f);
        }
    }
}

void LissajousSimulation::DrawCurveLines()
{
    const int paletteSize = ColorPalettes::GetSize();
    size_t n = curvePoints.size();
    if (n < 2) return;
    for (size_t i = 0; i + 1 < n; ++i) {
        float t = static_cast<float>(i) / static_cast<float>(n - 1);
        Color c = ColorPalettes::GetColorAt(t, paletteSize);
        Shapes::DrawLine(curvePoints[i].x, curvePoints[i].y,
                         curvePoints[i + 1].x, curvePoints[i + 1].y, c, 1.0f);
    }
}

void LissajousSimulation::DrawMarkers()
{
    const int paletteSize = ColorPalettes::GetSize();
    size_t n = curvePoints.size();
    for (size_t i = 0; i < n; i += 60) {
        float t = static_cast<float>(i) / static_cast<float>(n - 1);
        Color markerColor = ColorPalettes::GetColorAt(t, paletteSize);
        Shapes::DrawCircle(curvePoints[i].x, curvePoints[i].y, 3.0f,
                           markerColor, OriginMode::Center);
    }
}

void LissajousSimulation::DrawAxes()
{
    Color axisColor = Colors::LightGray;
    Shapes::DrawRectOutline(rectX, rectY, rectW, rectH, axisColor);
    Shapes::DrawLine(centerX, rectY, centerX, rectY + rectH, axisColor);
    Shapes::DrawLine(rectX, centerY, rectX + rectW, centerY, axisColor);
}

void LissajousSimulation::DrawCurrentPoint(float alpha)
{
    if (curvePoints.empty()) return;
    float angle  = Math::Lerp(prevPointAngle, pointAngle, alpha);
    float t      = Math::Mod(angle, Math::TWO_PI) / Math::TWO_PI;
    float idxF   = t * static_cast<float>(curvePoints.size() - 1);
    int   idx    = Math::FloorToInt(idxF);
    int   next   = Math::MinValue(idx + 1, static_cast<int>(curvePoints.size()) - 1);
    Math::Vec2f pos  = Math::Lerp(curvePoints[idx], curvePoints[next], idxF - idx);
    Shapes::DrawCircle(pos.x, pos.y, 10, Colors::RayYellow, OriginMode::Center);
}

void LissajousSimulation::DrawUI()
{
    const int lineHeight = 30;
    float y = 30.0f;
    const auto& currentPresetData = LissajousPresets::Get(currentPreset);

    Shapes::DrawRect(12.0f, 12.0f, 435.0f, 205.0f, {0, 0, 0, 130}, OriginMode::TopLeft);
    Shapes::DrawRectOutline(12.0f, 12.0f, 435.0f, 205.0f, {70, 100, 200, 45});

    Text::RenderText("Frequency X: " + FormatFloat(frequencyX, 2) + " (Left/Right)",
                     30, y, 20, Color{150, 255, 100}, OriginMode::TopLeft);
    y += lineHeight;

    Text::RenderText("Frequency Y: " + FormatFloat(frequencyY, 2) + " (Up/Down)",
                     30, y, 20, Color{255, 200, 100}, OriginMode::TopLeft);
    y += lineHeight;

    float phaseDegrees = Math::Mod(phaseShift * 180.0f / Math::PI, 360.0f);
    Text::RenderText("Phase: " + FormatFloat(phaseDegrees, 1) + "\xC2\xB0 | Speed: " +
                     FormatFloat(phaseRotationSpeed, 3) + " (P/O dir, +/- speed)",
                     30, y, 20, Color{100, 200, 255}, OriginMode::TopLeft);
    y += lineHeight;

    Text::RenderText("Ratio Fx:Fy = " + LissajousPresets::GetRatioString(frequencyX, frequencyY),
                     30, y, 20, Color{255, 150, 200}, OriginMode::TopLeft);
    y += lineHeight;

    Text::RenderText("Preset [" + std::to_string(currentPreset + 1) + "/" +
                     std::to_string(LissajousPresets::Count()) + "]: " + currentPresetData.name,
                     30, y, 20, Color{200, 100, 255}, OriginMode::TopLeft);
    y += lineHeight;

    Text::RenderText("Res: " + std::to_string(resolution) + " | FPS: " +
                     std::to_string(Time::GetFPS()),
                     30, y, 20, Color{200, 255, 100}, OriginMode::TopLeft);

    {
        const std::string hint =
            "Arrows Freq  |  P/O Dir  |  +/- Speed"
            "1-6 Presets  |  R Reset";

        // Split into lines to measure multiline text
        std::vector<std::string> lines;
        {
            std::istringstream iss(hint);
            std::string ln;
            while (std::getline(iss, ln)) lines.push_back(ln);
        }

        Fonts::FontHandle fh = FontManager::GetDefaultHandle();
        const auto* font = FontManager::GetFont(fh);

        float renderFontSize = 24.0f;
        float spacing = 0.0f;
        float scale = (font && font->GetSize() > 0) ? (renderFontSize / static_cast<float>(font->GetSize())) : 1.0f;

        float maxW = 0.0f;
        for (const auto& l : lines) {
            std::string m = l.empty() ? " " : l;
            Math::Vec2f d = FontManager::MeasureText(fh, m, spacing);
            maxW = std::max(maxW, d.x * scale);
        }

        float lineH = (font ? font->GetLineHeight() * scale : renderFontSize);
        float totalH = lineH * static_cast<float>(lines.size());

        float pad = 12.0f;
        float boxW = maxW + pad * 2.0f;
        float boxH = totalH + pad * 2.0f;

        float centerX = static_cast<float>(window().GetWidth()) * 0.5f;
        float bottomY = static_cast<float>(window().GetHeight()) - 40.0f;

        Shapes::DrawRect(centerX, bottomY, boxW, boxH, {0, 0, 0, 130}, OriginMode::Center);

        float startY = bottomY - (totalH * 0.5f) + (lineH * 0.5f);
        for (size_t i = 0; i < lines.size(); ++i) {
            Text::RenderTextEx(lines[i], centerX, startY + i * lineH, renderFontSize, spacing, Colors::LightGray, OriginMode::Center);
        }
    }
}

} // namespace RS
