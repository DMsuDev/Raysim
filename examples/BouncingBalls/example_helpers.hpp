#pragma once

#include "Raysim/Graphics/Color.hpp"
#include "Raysim/Math/Types/Vector2.hpp"

#include <string_view>

namespace RS::BouncingBallsExample {

// ============================================================================
// Background Configuration
// ============================================================================

/**
 * @struct BackgroundConfig
 * @brief Configuration for animated wave background visual effects
 * @details Controls the behavior of the rippling dot pattern, including wave propagation,
 *          pulsing intensity, and alpha channel fade effects.
 */
struct BackgroundConfig {
    /// Wave density - controls dot spacing pattern (lower = denser)
    float waveDistScale = 0.014f;

    /// Wave propagation speed multiplier (higher = faster animation)
    float waveTimeScale = 1.8f;

    /// Dot pulsing intensity multiplier relative to wave (1.0 = no pulsing)
    float dotSizeMultiplier = 3.2f;

    /// Base dot alpha value during wave animation
    float fadeAlphaMin = 55.0f;

    /// Peak dot alpha during wave crest
    float fadeAlphaMax = 70.0f;

    /// Edge fade-out rate (0.0-1.0, higher = steeper falloff)
    float fadeFalloff = 0.60f;

    /// Minimum alpha threshold to render dots (prevents noise)
    float dotMinAlpha = 8.0f;
};

/**
 * @struct BgTheme
 * @brief Color theme for the wave background
 * @details Defines HSV parameters for dynamic background coloring based on wave animation
 */
struct BgTheme {
    /// Theme name for display
    const char* name;

    /// Base hue value (0-360 degrees)
    float hueBase;

    /// Hue variation range applied by wave animation
    float hueRange;

    /// Saturation value (0.0-1.0)
    float saturation;
};

// ============================================================================
// Built-in Themes
// ============================================================================

/// Array of predefined color themes
static constexpr BgTheme BG_THEMES[] = {
    {"Blue", 220.0f, 60.0f, 0.65f},
    {"Cyan", 175.0f, 55.0f, 0.70f},
    {"Purple", 270.0f, 50.0f, 0.75f},
    {"Ember", 10.0f, 55.0f, 0.80f},
    {"Lime", 90.0f, 50.0f, 0.70f},
    {"Rainbow", 0.0f, 360.0f, 0.80f},
};

/// Number of available themes
static constexpr int NUM_BG_THEMES = std::size(BG_THEMES);

// ============================================================================
// Drawing Functions
// ============================================================================

/**
 * @brief Draw animated wave background with grid pattern
 *
 * Creates a mesmerizing wave effect by animating a grid of dots with rippling
 * patterns radiating from the center of the screen. Colors shift dynamically
 * based on the selected theme.
 *
 * @param windowWidth Viewport width in pixels
 * @param windowHeight Viewport height in pixels
 * @param themeIndex Index into BG_THEMES array (0 to NUM_BG_THEMES-1)
 * @param time Current simulation time in seconds
 * @param config Background configuration parameters (optional - uses defaults if omitted)
 *
 * @note Optimal performance with themeIndex 0-5; values beyond NUM_BG_THEMES wrap around
 * @note Grid size hardcoded to 70px for consistency; for dynamic sizing, pass as parameter
 */
void DrawBackground(int windowWidth, int windowHeight, int themeIndex, float time,
                    const BackgroundConfig& config = BackgroundConfig{});

/**
 * @brief Draw animated UI chip with text and bobbing effect
 *
 * Renders an interactive UI element with:
 * - Colored rounded rectangle background
 * - Semi-transparent shadow offset
 * - Outline border highlight
 * - Vertical bobbing animation
 * - Text label with contrasting color
 *
 * @param x X coordinate in pixels (TopLeft origin)
 * @param y Y coordinate in pixels (TopLeft origin)
 * @param text Display text (max recommended 20 chars for fit in 160px width)
 * @param col Main chip color (RGB values 0-255)
 * @param t Current time in seconds (drives bobbing animation)
 * @param sway Vertical sway amplitude in pixels (default 6.0f)
 *
 * @note Chip dimensions are fixed at 160×36 pixels
 * @note Text renders at 24px font size with fixed positioning
 * @note Bobbing frequency is 2.0 Hz; adjust t * 2.0f multiplier to change speed
 *
 * @example DrawChip(10, 10, "Score: 1000", {70, 130, 180, 180}, Time::GetTime());
 */
void DrawChip(float x, float y, std::string_view text, const RS::Color& col, float t, float sway = 6.0f);

} // namespace RS::BouncingBallsExample
