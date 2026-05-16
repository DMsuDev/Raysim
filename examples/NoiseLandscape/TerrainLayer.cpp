#include "TerrainLayer.hpp"
#include <Raysim/Raysim.hpp>

using namespace RS;

//==============================================================================
// NoiseName
//==============================================================================

const char* NoiseName(NoiseMode m)
{
    switch (m) {
        case NoiseMode::Perlin:   return "Perlin Noise";
        case NoiseMode::Simplex:  return "Simplex Noise";
        case NoiseMode::Cellular: return "Cellular Noise";
        case NoiseMode::Value:    return "Value Noise";
        case NoiseMode::FBM:      return "Fractal Brownian Motion";
    }
    RS_CORE_ASSERT(false, "Unhandled NoiseMode value");
    return "Unknown";
}

//==============================================================================
// TerrainLayer
//==============================================================================

TerrainLayer::TerrainLayer(float screenW, float screenH, int columnW)
    : screenW_(screenW), screenH_(screenH), columnW_(columnW)
{
    RS_ASSERT(screenW > 0.0f && screenH > 0.0f, "Screen dimensions must be positive");
    RS_ASSERT(columnW > 0, "Column width must be > 0");
}

void TerrainLayer::Reseed()
{
    // New noise seed -> completely different landscape on next draw.
    Math::Random::SeedRandom();
}

//------------------------------------------------------------------------------
// Noise sampling - routes to the selected algorithm
//------------------------------------------------------------------------------

float TerrainLayer::SampleNoise(float worldX) const
{
    float nx = worldX * NOISE_SCALE;
    float ny = 0.5f; // fixed Y-slice for a 1D cross-section

    switch (mode_) {
        case NoiseMode::Perlin:
            return Math::Random::PerlinNoise(nx, ny);

        case NoiseMode::Simplex:
            return Math::Random::SimplexNoise(nx, ny);

        case NoiseMode::Cellular:
            // CellularNoise returns [0,1]; remap to [-1,1] to match other modes
            return Math::Random::CellularNoise(nx, ny) * 2.0f - 1.0f;

        case NoiseMode::Value:
            return Math::Random::ValueNoise(nx, ny) * 2.0f - 1.0f;

        case NoiseMode::FBM:
            return Math::Random::FractalBrownianMotion(nx, ny, 6, 0.5f, 2.0f);
    }
    return 0.0f;
}

//------------------------------------------------------------------------------
// Parallax far ridgeline
//------------------------------------------------------------------------------

void TerrainLayer::DrawFarRidgeline(float skyH) const
{
    float bgScroll = scrollX_ * BG_PARALLAX;
    int   cols     = static_cast<int>(screenW_) / columnW_ + 2;

    constexpr float RIDGE_FREQ = 3.0f; // crests per noise unit

    for (int col = 0; col < cols; ++col) {
        float wx = bgScroll + static_cast<float>(col * columnW_);

        // Distinct Y-slice (1.5) prevents correlation with foreground
        float noise = Math::Random::PerlinNoise(wx * BG_SCALE * RIDGE_FREQ, 1.5f);
        float t     = std::clamp(noise * 0.5f + 0.5f, 0.0f, 1.0f);
        float top   = skyH - t * skyH * 0.50f;

        Shapes::DrawRect(static_cast<float>(col * columnW_), top,
                         static_cast<float>(columnW_), screenH_ - top,
                         RIDGE_COLOR);
    }
}

//------------------------------------------------------------------------------
// Main terrain with natural layered colouring
//------------------------------------------------------------------------------

void TerrainLayer::DrawTerrain(float skyH) const
{
    const int cols = static_cast<int>(screenW_) / columnW_ + 2;

    for (int col = 0; col < cols; ++col)
    {
        const float worldX   = scrollX_ + static_cast<float>(col * columnW_);
        const float screenX  = static_cast<float>(col * columnW_);

        float noise    = SampleNoise(worldX);
        float terrainY = skyH - noise * (skyH * 0.4f);
        float groundH  = screenH_ - terrainY;
        if (groundH <= 0.0f) continue;

        Shapes::DrawRect(screenX, terrainY,
                         static_cast<float>(columnW_), CAP_HEIGHT, GRASS_COLOR);

        // Underground bands: lerp from light dirt to dark bedrock
        float belowCap = groundH - CAP_HEIGHT;
        if (belowCap <= 0.0f) continue;

        const float bandHeight = belowCap / static_cast<float>(UNDERGROUND_STEPS);

        for (int i = 0; i < UNDERGROUND_STEPS; ++i) {
            float t = static_cast<float>(i) / static_cast<float>(UNDERGROUND_STEPS - 1);
            Color bandColor = Color::Lerp(DIRT_LIGHT, DIRT_DARK, t);

            float y = terrainY + CAP_HEIGHT + i * bandHeight;

            Shapes::DrawRect(screenX, y,
                             static_cast<float>(columnW_), bandHeight + 1.0f, bandColor);
        }
    }
}
