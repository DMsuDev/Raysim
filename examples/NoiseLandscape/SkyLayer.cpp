#include "SkyLayer.hpp"
#include <Raysim/Raysim.hpp>

using namespace RS;

//==============================================================================
// Construction
//==============================================================================

SkyLayer::SkyLayer(float width, float skyH, int starCount, int steps)
    : starCount_(starCount), gradientSteps_(steps), width_(width), skyHeight_(skyH)
{
    RS_ASSERT(width > 0.0f && skyH > 0.0f, "Sky dimensions must be positive");
    RS_ASSERT(starCount >= 0, "Star count must be non-negative");
    RS_ASSERT(steps > 0, "Gradient steps must be > 0");
    SpawnStars();
}

//==============================================================================
// Drawing
//==============================================================================

void SkyLayer::DrawSky() const
{
    const float bandHeight = skyHeight_ / static_cast<float>(gradientSteps_);

    // Vertical gradient from deep indigo (top) to warmer purple (horizon)
    for (int i = 0; i < gradientSteps_; ++i) {
        // Interpolation factor for the gradient band
        // -1 gradientSteps_ - 1 to ensure the last band reaches SKY_BOTTOM exactly at skyHeight_
        float t = static_cast<float>(i) / static_cast<float>(gradientSteps_ - 1);
        float y = t * skyHeight_;

        Color band = Color::Lerp(SKY_TOP, SKY_BOTTOM, t);
        Shapes::DrawRect(0.0f, y, width_, bandHeight + 1.0f, band);
    }
}

void SkyLayer::DrawStars() const
{
    float time = Time::GetTime();
    for (const auto& s : stars_) {

        // Culling - skip stars outside the sky area (should be rare since they are spawned within it, but just in case)
        if (s.y >= skyHeight_ || s.x < 0.0f || s.x > width_)
            continue;

        // Sinusoidal twinkle modulating brightness (0.5 to 1.0)
        const float twinkle = 0.5f + 0.5f * std::sin(s.twinklePhase + time * s.twinkleSpeed);

        // Alpha based on base brightness and twinkle, clamped to [0, 255]
        const float brightness = Math::Clamp01(s.brightness * twinkle);
        const unsigned char alpha = static_cast<unsigned char>(brightness * 255.0f);

        const float size = 1.0f + s.brightness * 2.0f;
        const float half = size * 0.5f;

        Shapes::DrawRect(s.x - half, s.y - half, size, size, {255, 255, 255, alpha});
    }
}

void SkyLayer::DrawMoon() const
{
    float mx = width_ * MOON_X_RATIO;
    float my = skyHeight_  * MOON_Y_RATIO;

    // Concentric soft glow halos
    Shapes::DrawCircle(mx, my, 40.0f, {MOON_GLOW.r, MOON_GLOW.g, MOON_GLOW.b, 12}, OriginMode::Center);
    Shapes::DrawCircle(mx, my, 28.0f, {MOON_GLOW.r, MOON_GLOW.g, MOON_GLOW.b, 28}, OriginMode::Center);

    // Bright disc
    Shapes::DrawCircle(mx, my, 18.0f, MOON_DISC, OriginMode::Center);
}

//==============================================================================
// Star generation
//==============================================================================

void SkyLayer::SpawnStars()
{
    stars_.clear();
    stars_.reserve(static_cast<size_t>(starCount_));
    for (int i = 0; i < starCount_; ++i) {
        Star s;
        s.x            = Math::Random::Float(width_);
        s.y            = Math::Random::Float(skyHeight_ * 0.66f);
        s.brightness   = Math::Random::Range(0.25f, 1.0f);
        s.twinklePhase = Math::Random::Range(0.0f, Math::TWO_PI);
        s.twinkleSpeed = Math::Random::Range(0.4f, 3.0f);
        stars_.push_back(s);
    }
}
