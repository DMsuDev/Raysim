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

} // namespace RS
