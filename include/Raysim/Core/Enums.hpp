#pragma once

namespace RS {

/**
 * @enum OriginMode
 * @brief Defines the origin point (anchor) for drawing operations
 *
 * Specifies how coordinates are interpreted when drawing shapes and text.
 * Each draw function uses TopLeft (classic raylib behavior) by default when
 * calling the basic overload, but you can specify an alternative origin.
 *
 * The origin modes form a 3×3 grid:
 * ```
 * TopLeft    — TopCenter    — TopRight
 *    |             |             |
 * CenterLeft —  Center     — CenterRight
 *    |             |             |
 * BottomLeft — BottomCenter — BottomRight
 * ```
 *
 * Examples:
 * - TopLeft: Traditional behavior. DrawRect(100, 100, ...) draws top-left corner at (100, 100)
 * - Center: Most game-friendly. DrawRect(100, 100, ...) draws center at (100, 100)
 * - BottomCenter: Useful for UI text. DrawText(..., OriginMode::BottomCenter) anchors below
 *
 * @note Default behavior (when not specifying origin) is TopLeft for backward compatibility
 * @see Renderer::DrawRect(), Renderer::DrawCircle(), Renderer::DrawText()
 */
enum class OriginMode {
    TopLeft,
    TopCenter,
    TopRight,
    CenterLeft,
    Center,       // Default
    CenterRight,
    BottomLeft,
    BottomCenter,
    BottomRight
};

} // namespace RS
