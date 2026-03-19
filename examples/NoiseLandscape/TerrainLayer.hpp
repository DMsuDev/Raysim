#pragma once

#include <Raysim/Graphics/Color.hpp>

/// Noise algorithms available for terrain generation.
enum class NoiseMode { Perlin, Simplex, Cellular, Value, FBM };

/// Human-readable label for a NoiseMode.
const char* NoiseName(NoiseMode m);

/// Procedural scrolling terrain with height-based coloring and parallax ridgeline.
///
/// Owns noise sampling, scroll offset, and column-based drawing.  The caller
/// advances the scroll with SetScrollX() and passes the sky-horizon height
/// before each draw call.
class TerrainLayer
{
public:
    TerrainLayer(float screenW, float screenH, int columnW = 3);

    // --- Configuration ------------------------------------------------------

    void SetMode(NoiseMode mode)    { mode_ = mode; }
    void SetScrollX(float x)        { scrollX_ = x; }
    void SetScreenSize(float w, float h) { screenW_ = w; screenH_ = h; }

    NoiseMode GetMode()    const { return mode_; }
    float     GetScrollX() const { return scrollX_; }

    /// Re-randomise the noise seed so the landscape changes completely.
    void Reseed();

    // --- Drawing ------------------------------------------------------------

    /// Parallax far ridgeline (draw before foreground terrain).
    void DrawFarRidgeline(float skyH) const;

    /// Main scrolling terrain with height-based cap and layered underground.
    void DrawTerrain(float skyH) const;

private:
    float SampleNoise(float worldX) const;

    float     screenW_;
    float     screenH_;
    int       columnW_;
    float     scrollX_   = 0.0f;
    NoiseMode mode_      = NoiseMode::FBM;

    // --- Noise tuning -------------------------------------------------------
    static constexpr float NOISE_SCALE       = 0.003f;  // world-units -> noise space
    static constexpr float BG_SCALE          = 0.001f;  // background ridgeline frequency
    static constexpr float BG_PARALLAX       = 0.35f;   // parallax speed ratio

    // --- Terrain layer counts -----------------------------------------------
    static constexpr int   UNDERGROUND_STEPS = 8;       // vertical dirt/rock bands
    static constexpr float CAP_HEIGHT        = 5.0f;    // surface cap strip height

    // --- Colour palette (top surface) ---------------------------------------
    static constexpr RS::Color SNOW_COLOR  {220, 230, 245, 255};
    static constexpr RS::Color ROCK_COLOR  {110, 100,  90, 255};
    static constexpr RS::Color GRASS_COLOR { 58, 140,  48, 255};

    // --- Underground gradient endpoints (light -> dark) ---------------------
    static constexpr RS::Color DIRT_LIGHT  {130,  95,  60, 255};
    static constexpr RS::Color DIRT_DARK   { 45,  30,  18, 255};

    // --- Far ridgeline colour -----------------------------------------------
    static constexpr RS::Color RIDGE_COLOR { 22,  28,  55, 190};
};
