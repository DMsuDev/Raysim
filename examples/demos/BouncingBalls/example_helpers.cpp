#include "example_helpers.hpp"
#include "Raysim/Graphics.hpp"
#include "Raysim/Core/Time.hpp"
#include "Raysim/Math/Utils/MathUtils.hpp"

#include <spdlog/fmt/fmt.h>

namespace RS::BouncingBallsExample {

// ============================================================================
// Constants
// ============================================================================

/// Standard grid cell size for background pattern (pixels)
static constexpr float GRID_SIZE = 70.0f;

/// Background wave horizontal scroll speed (pixels/second)
static constexpr float BG_WAVE_SPEED_X = 5.0f;

/// Background wave vertical scroll speed (pixels/second)
static constexpr float BG_WAVE_SPEED_Y = 3.5f;

/// Chip dimensions (width x height in pixels)
static constexpr float CHIP_WIDTH = 160.0f;
static constexpr float CHIP_HEIGHT = 36.0f;

/// Shadow offset from chip position (pixels)
static constexpr float SHADOW_OFFSET = 2.0f;

/// Chip text rendering position offset from chip origin (pixels)
static constexpr float TEXT_OFFSET_X = 12.0f;
static constexpr float TEXT_OFFSET_Y = 8.0f;

/// Chip text font size (pixels)
static constexpr int CHIP_FONT_SIZE = 24;

// ============================================================================
// Drawing Functions
// ============================================================================

void DrawBackground(int windowWidth, int windowHeight, int themeIndex, float time,
                    const BackgroundConfig& config)
{
    // Clamp theme index to valid range
    const int safeThemeIndex = themeIndex % NUM_BG_THEMES;
    const BgTheme& theme = BG_THEMES[safeThemeIndex];

    // Screen center point (slightly elevated for visual interest)
    Math::Vec2f center{windowWidth * 0.5f, windowHeight * 0.45f};

    // Calculate fade radius for edge vignette effect
    float maxRadius = Math::MaxValue(static_cast<float>(windowWidth), static_cast<float>(windowHeight)) * 0.95f;
    float fadeRadius = maxRadius * config.fadeFalloff;

    // Calculate scrolling offset for animated grid
    float offsetX = Math::Mod(time * BG_WAVE_SPEED_X, GRID_SIZE);
    float offsetY = Math::Mod(time * BG_WAVE_SPEED_Y, GRID_SIZE);

    unsigned char minAlpha = static_cast<unsigned char>(config.dotMinAlpha);

    // Draw grid of pulsing dots with wave propagation effect
    for (float gx = offsetX - GRID_SIZE; gx < windowWidth + GRID_SIZE; gx += GRID_SIZE) {
        for (float gy = offsetY - GRID_SIZE; gy < windowHeight + GRID_SIZE; gy += GRID_SIZE) {
            // Relative position from screen center
            Math::Vec2f pos{gx - center.x, gy - center.y};
            float distance = pos.Length();

            // Wave animation: creates concentric ripple effect from center
            // PingPong creates smooth oscillation (0→1→0) at frequency controlled by waveTimeScale
            float wave = Math::PingPong(distance * config.waveDistScale - time * config.waveTimeScale, 1.0f);

            // Edge fade effect: smooth falloff toward screen edges (vignette)
            float edgeFade = Math::Clamp(1.0f - distance / fadeRadius, 0.0f, 1.0f);

            // Dot size pulsation based on wave phase
            float dotRadius = 1.0f + wave * config.dotSizeMultiplier;

            // Combined alpha: base fade on wave + intensity variation from edge
            auto alpha = static_cast<unsigned char>(edgeFade * config.fadeAlphaMin + wave * config.fadeAlphaMax);

            // Only render if alpha exceeds threshold (prevents visual noise)
            if (alpha > minAlpha) {
                // Dynamic hue based on wave phase for color cycling effect
                float hue = Math::Mod(theme.hueBase + wave * theme.hueRange, 360.0f);
                Shapes::DrawCircle(gx, gy, dotRadius,
                                 Color::FromHSV(hue, theme.saturation, 1.0f, alpha),
                                 OriginMode::Center);
            }
        }
    }
}

void DrawChip(float x, float y, std::string_view text, const Color& col, float t, float sway)
{
    // Animated bobbing motion with time-based phase
    // PingPong creates smooth vertical oscillation (0 to sway*2 and back)
    float bob = Math::PingPong(t * 2.0f + x * 0.01f, sway * 2.0f) - sway;

    // Draw drop shadow (dark, semi-transparent)
    Shapes::DrawRect(x + SHADOW_OFFSET, y + bob + SHADOW_OFFSET, CHIP_WIDTH, CHIP_HEIGHT,
                    {0, 0, 0, 90}, OriginMode::TopLeft);

    // Draw main chip background with provided color
    Shapes::DrawRect(x, y + bob, CHIP_WIDTH, CHIP_HEIGHT,
                    {col.r, col.g, col.b, 160}, OriginMode::TopLeft);

    // Draw bright border outline for visual definition
    Shapes::DrawRectOutline(x, y + bob, CHIP_WIDTH, CHIP_HEIGHT, {255, 255, 255, 30});

    // Render text label with high contrast (light color on dark background)
    Text::RenderText(text.data(), x + TEXT_OFFSET_X, y + bob + TEXT_OFFSET_Y,
                    CHIP_FONT_SIZE, {245, 245, 245}, OriginMode::TopLeft);
}

} // namespace RS::BouncingBallsExample
