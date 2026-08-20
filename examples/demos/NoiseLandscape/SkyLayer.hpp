#pragma once

#include <Raysim/Graphics/Color.hpp>
#include <vector>

struct Star
{
    float x, y;
    float brightness;
    float twinklePhase;
    float twinkleSpeed;
};

/// Sky background with gradient, twinkling star field, and moon.
///
/// Stars are placed randomly in the upper sky region and animated with
/// individual brightness and twinkle speed.
class SkyLayer
{
public:
    SkyLayer() = default;
    SkyLayer(float width, float skyHeight, int starCount = 200, int steps = 24);

    void DrawSky()   const;
    void DrawStars() const;
    void DrawMoon()  const;

    /// Re-randomise the star field (call after re-seeding the RNG).
    void Reseed() { SpawnStars(); }

    void SetSkyHeight(float h) { skyHeight_ = h; }
    void SetWidth(float w)     { width_ = w; }

private:
    void SpawnStars();

    int   starCount_ {200};
    int   gradientSteps_ {24};
    float width_     {100.0f};
    float skyHeight_ {100.0f};

    std::vector<Star> stars_;

    // ========================================================================
    // Sky gradient endpoints (top to horizon)
    // ========================================================================
    static constexpr RS::Color SKY_TOP     {  8, 10, 38, 255};
    static constexpr RS::Color SKY_BOTTOM  { 55, 22, 72, 255};

    // ========================================================================
    // Moon
    // ========================================================================
    static constexpr float MOON_X_RATIO = 0.80f;
    static constexpr float MOON_Y_RATIO = 0.20f;
    static constexpr RS::Color MOON_GLOW {255, 240, 175, 255}; // alpha set per halo
    static constexpr RS::Color MOON_DISC {245, 238, 200, 240};
};
