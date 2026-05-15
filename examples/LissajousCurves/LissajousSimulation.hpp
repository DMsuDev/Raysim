#pragma once

#include "LissajousTypes.hpp"

namespace RS
{

/**
 * @brief Lissajous curve visualization scene
 *
 * Renders parametric curves defined by:
 * x(t) = A * sin(fx*t + phase)
 * y(t) = A * sin(fy*t)
 *
 * Controls:
 * - Arrow keys: Adjust frequencies X/Y
 * - P/O: Phase rotation direction
 * - +/-: Phase rotation speed
 * - 1-6: Select preset curves
 * - R: Reset to defaults
 */
class LissajousSimulation : public Scene
{
    RS_SCENE(LissajousSimulation)

public:
    void OnStart() override;
    void OnUpdate(float dt) override;
    void OnFixedUpdate(float fixedDt) override;
    void OnDraw(float alpha) override;

private:
    void UpdatePhase(float fixedDt);
    void SmoothFrequencies(float dt);
    void GenerateCurve();
    void DrawCurveTrails();
    void DrawCurveLines();
    void DrawMarkers();
    void DrawAxes();
    void DrawCurrentPoint(float alpha);
    void DrawUI();

    float frequencyX = 3.0f;
    float frequencyY = 2.0f;
    float phaseShift = 0.0f;
    float phaseRotationSpeed = 0.2f;

    std::vector<Math::Vec2f> curvePoints;
    int resolution = 1200;
    std::vector<float> paramT;

    float smoothFrequencyX = 3.0f;
    float smoothFrequencyY = 2.0f;
    float freqSmoothSpeed = 8.0f;

    float prevPointAngle = 0.0f;
    float pointAngle = 0.0f;
    const float markerSpeed = 2.0f;

    float centerX = 0.0f;
    float centerY = 0.0f;
    float curveRadius = 0.0f;
    float rectX = 0.0f;
    float rectY = 0.0f;
    float rectW = 0.0f;
    float rectH = 0.0f;

    int currentPreset = 3;

    bool curveDirty = true;
    float cachedPhaseShift = 0.0f;
    float cachedFreqX = 0.0f;
    float cachedFreqY = 0.0f;
};

} // namespace RS
