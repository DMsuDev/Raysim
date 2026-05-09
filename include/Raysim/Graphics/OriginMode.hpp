#pragma once

#include "Raysim/Math/Types/Vector2.hpp"

namespace RS {

/**
 * @enum OriginMode
 * @brief Defines the origin point (anchor) for drawing operations
 *
 * Specifies how coordinates are interpreted when drawing shapes and text.
 * Each draw function uses TopLeft (classic raylib behavior) by default when
 * calling the basic overload, but you can specify an alternative origin.
 *
 * The origin modes form a 3x3 grid:
 * ```
 * TopLeft    - TopCenter    - TopRight
 *    |             |             |
 * CenterLeft -  Center     - CenterRight
 *    |             |             |
 * BottomLeft - BottomCenter - BottomRight
 * ```
 *
 * @note Default behavior (when not specifying origin) depends on the backend/renderer.
 * For example, raylib's default is TopLeft, while some other backends may use Center.
 */
enum class OriginMode {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

// ============================================================================
// Helper Functions
// ============================================================================

/// @brief Convert OriginMode to a normalized origin vector in [0..1].
///
/// Maps each OriginMode to a 2D offset vector where:
/// - x in [0, 1]: 0=left, 0.5=center, 1=right
/// - y in [0, 1]: 0=top, 0.5=center, 1=bottom
///
/// @param origin The OriginMode to convert.
/// @return A Vec2 representing the normalized offset.
///
/// Example:
/// @code
/// auto v = OriginToVector(OriginMode::Center);  // Returns {0.5f, 0.5f}
/// auto v = OriginToVector(OriginMode::TopLeft); // Returns {0.0f, 0.0f}
/// @endcode
[[nodiscard]] constexpr Math::Vec2f OriginToVector(OriginMode origin) noexcept
{
    float x = 0.0f;
    float y = 0.0f;

    switch (origin) {
        case OriginMode::TopLeft:      x = 0.0f; y = 0.0f; break;
        case OriginMode::TopCenter:    x = 0.5f; y = 0.0f; break;
        case OriginMode::TopRight:     x = 1.0f; y = 0.0f; break;
        case OriginMode::CenterLeft:   x = 0.0f; y = 0.5f; break;
        case OriginMode::Center:       x = 0.5f; y = 0.5f; break;
        case OriginMode::CenterRight:  x = 1.0f; y = 0.5f; break;
        case OriginMode::BottomLeft:   x = 0.0f; y = 1.0f; break;
        case OriginMode::BottomCenter: x = 0.5f; y = 1.0f; break;
        case OriginMode::BottomRight:  x = 1.0f; y = 1.0f; break;
        default:                       x = 0.0f; y = 0.0f; break;
    }

    return { x, y };
}

} // namespace RS
