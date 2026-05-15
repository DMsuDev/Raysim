#pragma once

#include "Raysim/Raysim.hpp"

#include <string>
#include <vector>

namespace RS
{

struct LissajousPreset
{
    std::string name;
    float frequencyX;
    float frequencyY;
    std::string description;
};

struct ColorPalette
{
    std::vector<Color> colors;
};

class LissajousPresets
{
public:
    static const std::vector<LissajousPreset>& GetAll();
    static const LissajousPreset& Get(size_t index);
    static size_t Count();
    static int FindPreset(float freqX, float freqY);
    static std::string GetRatioString(float freqX, float freqY);
};

class ColorPalettes
{
public:
    static const ColorPalette& GetDefault();
    static const std::vector<Color>& GetColors();
    static Color GetColorAt(float t, int paletteSize);
    static int GetSize();
};

class LissajousCurveGenerator
{
public:
    LissajousCurveGenerator();

    void SetResolution(int res);
    void SetParameters(float freqX, float freqY, float phase, float radius, float cx, float cy);
    void Update(float smoothFreqX, float smoothFreqY, float phase, float radius, float cx, float cy);
    const std::vector<Math::Vec2f>& GetPoints() const { return points; }
    const std::vector<float>& GetParams() const { return params; }
    int GetResolution() const { return resolution; }
    bool IsDirty() const { return dirty; }
    void ClearDirty() { dirty = false; }

private:
    void Generate();
    int resolution;
    std::vector<Math::Vec2f> points;
    std::vector<float> params;
    float cachedFreqX, cachedFreqY, cachedPhase, cachedRadius, cachedCX, cachedCY;
    bool dirty;
};

class LissajousRenderer
{
public:
    LissajousRenderer();

    void SetCurveData(const std::vector<Math::Vec2f>* points, const std::vector<float>* params,
                      int resolution, float radius, float cx, float cy, float freqX, float freqY, float phase);

    void RenderBackground() const;
    void RenderCurveTrails(float phase, float smoothFreqX, float smoothFreqY) const;
    void RenderCurveLines() const;
    void RenderMarkers() const;
    void RenderAxes(float rectX, float rectY, float rectW, float rectH) const;
    void RenderCurrentPoint(float prevAngle, float currAngle, float alpha) const;

    void SetCenter(float cx, float cy);
    void SetRect(float x, float y, float w, float h);

private:
    const std::vector<Math::Vec2f>* curvePoints;
    const std::vector<float>* paramT;
    int resolution;
    float curveRadius, centerX, centerY, smoothFreqX, smoothFreqY, phaseShift;
    float rectX, rectY, rectW, rectH;
    float prevPointAngle, pointAngle;
};

struct LissajousState
{
    float frequencyX = 3.0f;
    float frequencyY = 2.0f;
    float phaseShift = 0.0f;
    float phaseRotationSpeed = 0.2f;
    float smoothFreqX = 3.0f;
    float smoothFreqY = 2.0f;
    int currentPreset = 3;
    int resolution = 1200;
};

struct LissajousViewport
{
    float centerX = 0.0f;
    float centerY = 0.0f;
    float curveRadius = 0.0f;
    float rectX = 0.0f;
    float rectY = 0.0f;
    float rectW = 0.0f;
    float rectH = 0.0f;
};

} // namespace RS
