#pragma once

#include "Color.hpp"
#include "Raysim/Math/Vector2.hpp"
#include "OriginMode.hpp"

namespace RS {

/**
 * @namespace Shapes
 * @brief 2D shape drawing API with flexible coordinate origin support
 *
 * Provides functions for drawing filled and outlined shapes (rectangles, circles, lines, etc.)
 * with optional OriginMode parameter for flexible coordinate systems.
 * Without specifying origin, functions use TopLeft (traditional raylib behavior).
 *
 * Key Features:
 * - Origin Mode Support: Draw anchored to center, corners, or edges
 * - Flexible Overloads: Each draw function has versions with/without OriginMode
 * - Color Abstraction: Uses RS::Color instead of raylib's internal representation
 *
 * @example
 * // Draw rectangle centered at (200, 200)
 * Shapes::DrawRect(200, 200, 50, 50, Color::Red(), OriginMode::Center);
 *
 * // Traditional top-left anchoring (no origin mode)
 * Shapes::DrawRect(10, 10, 50, 50, Color::Blue());
 */
namespace Shapes {

//==============================================================================
// Rectangle
//==============================================================================

    /**
     * @brief Draw a filled rectangle with top-left at specified coordinates
     *
     * Traditional rectangle drawing with top-left corner anchored at (x, y).
     * For flexible positioning, use the OriginMode overload.
     *
     * @param x X-coordinate of top-left corner
     * @param y Y-coordinate of top-left corner
     * @param w Width in pixels
     * @param h Height in pixels
     * @param color Fill color
     * @see DrawRect(float, float, float, float, const Color&, OriginMode)
     */
    void DrawRect(float x, float y, float w, float h, const Color& color);

    /**
     * @brief Draw a filled rectangle with custom origin point
     *
     * Allows positioning the rectangle anchored to different points (center, corners, edges).
     * The (x, y) coordinates correspond to the position specified by @c origin.
     *
     * @param x X-coordinate of the anchor point
     * @param y Y-coordinate of the anchor point
     * @param w Width in pixels
     * @param h Height in pixels
     * @param color Fill color
     * @param origin Which point of the rectangle is at (x, y)
     * @example
     * // Draw 50×50 rectangle centered at (200, 150)
     * Shapes::DrawRect(200, 150, 50, 50, Color::Green(), OriginMode::Center);
     *
     * // Draw in bottom-left corner of screen
     * Shapes::DrawRect(10, 470, 100, 20, Color::Black(), OriginMode::BottomLeft);
     * @see OriginMode
     */
    void DrawRect(float x, float y, float w, float h, const Color& color, OriginMode origin);

    /**
     * @brief Draw a rectangle outline with top-left at specified coordinates
     *
     * Draws only the outline (border) of a rectangle. Thickness controls the border width.
     * For flexible positioning, use the OriginMode overload.
     *
     * @param x X-coordinate of top-left corner
     * @param y Y-coordinate of top-left corner
     * @param w Width in pixels
     * @param h Height in pixels
     * @param color Outline color
     * @param thickness Border width in pixels (default: 1.0f)
     * @see DrawRectOutline(float, float, float, float, const Color&, float, OriginMode)
     */
    void DrawRectOutline(float x, float y, float w, float h, const Color& color, float thickness = 1.0f);

    /**
     * @brief Draw a rectangle outline with custom origin point
     *
     * Allows positioning the rectangle outline anchored to different points.
     * The (x, y) coordinates correspond to the position specified by @c origin.
     *
     * @param x X-coordinate of the anchor point
     * @param y Y-coordinate of the anchor point
     * @param w Width in pixels
     * @param h Height in pixels
     * @param color Outline color
     * @param thickness Border width in pixels
     * @param origin Which point of the rectangle is at (x, y)
     * @example
     * // Draw 100×50 outline centered at (250, 200) with 2px border
     * Shapes::DrawRectOutline(250, 200, 100, 50, Color::White(), 2.0f, OriginMode::Center);
     * @see OriginMode, DrawRect()
     */
    void DrawRectOutline(float x, float y, float w, float h, const Color& color, float thickness, OriginMode origin);

    /**
     * @brief Draw a filled rectangle with rounded corners
     *
     * Draws a rectangle with smoothly rounded corners. The roundness parameter
     * controls the radius of the corner rounding.
     *
     * @param x X-coordinate of top-left corner
     * @param y Y-coordinate of top-left corner
     * @param w Width in pixels
     * @param h Height in pixels
     * @param roundness Corner radius (0.0f = no rounding, 0.5f = very rounded)
     * @param color Fill color
     * @see DrawRectRounded(float, float, float, float, float, const Color&, OriginMode)
     */
    void DrawRectRounded(float x, float y, float w, float h, float roundness, const Color& color);

    /**
     * @brief Draw a filled rounded rectangle with custom origin point
     *
     * @param x X-coordinate of the anchor point
     * @param y Y-coordinate of the anchor point
     * @param w Width in pixels
     * @param h Height in pixels
     * @param roundness Corner radius (0.0f = sharp, 0.5f = very rounded)
     * @param color Fill color
     * @param origin Which point of the rectangle is at (x, y)
     * @example
     * // Draw rounded button centered at (300, 300)
     * Shapes::DrawRectRounded(300, 300, 120, 40, 0.2f, Color::SkyBlue(), OriginMode::Center);
     * @see OriginMode, DrawRect()
     */
    void DrawRectRounded(float x, float y, float w, float h, float roundness, const Color& color, OriginMode origin);


//==============================================================================
// Circle
//==============================================================================

    /**
     * @brief Draw a filled circle with center at specified coordinates
     *
     * @param x X-coordinate of circle center
     * @param y Y-coordinate of circle center
     * @param radius Circle radius in pixels
     * @param color Fill color
     * @see DrawCircle(float, float, float, const Color&, OriginMode)
     */
    void DrawCircle(float x, float y, float radius, const Color& color);

    /**
     * @brief Draw a filled circle with custom origin point
     *
     * Note: For circles, OriginMode::Center is most useful since circles are radially symmetric.
     * Other modes will position the bounding box of the circle differently.
     *
     * @param x X-coordinate of the anchor point
     * @param y Y-coordinate of the anchor point
     * @param radius Circle radius in pixels
     * @param color Fill color
     * @param origin Which point of the circle's bounding box is at (x, y)
     * @example
     * // Draw ball centered at (200, 200)
     * Shapes::DrawCircle(200, 200, 15, Color::Yellow(), OriginMode::Center);
     * @see OriginMode
     */
    void DrawCircle(float x, float y, float radius, const Color& color, OriginMode origin);

    /**
     * @brief Draw a circle outline (unfilled) with center at specified coordinates
     *
     * @param x X-coordinate of circle center
     * @param y Y-coordinate of circle center
     * @param radius Circle radius in pixels
     * @param color Outline color
     * @see DrawCircleOutline(float, float, float, const Color&, OriginMode)
     */
    void DrawCircleOutline(float x, float y, float radius, const Color& color);

    /**
     * @brief Draw a circle outline with custom origin point
     *
     * @param x X-coordinate of the anchor point
     * @param y Y-coordinate of the anchor point
     * @param radius Circle radius in pixels
     * @param color Outline color
     * @param origin Which point of the circle's bounding box is at (x, y)
     * @example
     * // Draw circle outline centered at (400, 250)
     * Shapes::DrawCircleOutline(400, 250, 30, Color::White(), OriginMode::Center);
     * @see OriginMode, DrawCircle()
     */
    void DrawCircleOutline(float x, float y, float radius, const Color& color, OriginMode origin);


//==============================================================================
// Primitives (line / triangle / polygon)
//==============================================================================

    /**
     * @brief Draw a line between two points
     *
     * @param x1 X-coordinate of start point
     * @param y1 Y-coordinate of start point
     * @param x2 X-coordinate of end point
     * @param y2 Y-coordinate of end point
     * @param color Line color
     * @param thickness Line width in pixels (default: 1.0f)
     * @example
     * Shapes::DrawLine(10, 10, 100, 100, Color::Red(), 2.0f);
     */
    void DrawLine(float x1, float y1, float x2, float y2, const Color& color, float thickness = 1.0f);

    /**
     * @brief Draw a filled triangle
     *
     * @param x1 X-coordinate of first vertex
     * @param y1 Y-coordinate of first vertex
     * @param x2 X-coordinate of second vertex
     * @param y2 Y-coordinate of second vertex
     * @param x3 X-coordinate of third vertex
     * @param y3 Y-coordinate of third vertex
     * @param color Fill color
     */
    void DrawTriangle(float x1, float y1, float x2, float y2, float x3, float y3, const Color& color);

    /**
     * @brief Draw a triangle outline (unfilled)
     *
     * @param x1 X-coordinate of first vertex
     * @param y1 Y-coordinate of first vertex
     * @param x2 X-coordinate of second vertex
     * @param y2 Y-coordinate of second vertex
     * @param x3 X-coordinate of third vertex
     * @param y3 Y-coordinate of third vertex
     * @param color Outline color
     */
    void DrawTriangleOutline(float x1, float y1, float x2, float y2, float x3, float y3, const Color& color);

    /**
     * @brief Draw a regular polygon (rhombus, triangle, square, pentagon, hexagon, etc.)
     *
     * Creates a regular polygon with @c sides sides, rotated by @c rotation degrees.
     *
     * @param x X-coordinate of center
     * @param y Y-coordinate of center
     * @param sides Number of sides (3 = triangle, 6 = hexagon, etc.)
     * @param radius Distance from center to vertex in pixels
     * @param rotation Rotation in degrees
     * @param color Fill color
     * @example
     * Shapes::DrawPolygon(200, 200, 6, 30, 0, Color::Purple());  // Hexagon
     * Shapes::DrawPolygon(200, 200, 3, 30, 0, Color::Green());   // Triangle
     */
    void DrawPolygon(float x, float y, int sides, float radius, float rotation, const Color& color);

//==============================================================================
// Point
//==============================================================================

    /**
     * @brief Draw a single pixel point
     *
     * @param x X-coordinate of the point
     * @param y Y-coordinate of the point
     * @param color Point color
     */
    void DrawPoint(float x, float y, const Color& color);

    /**
     * @brief Draw a point with custom size
     *
     * Draws a small circle to represent a point at specified coordinates.
     *
     * @param x X-coordinate of the point center
     * @param y Y-coordinate of the point center
     * @param size Point size in pixels (radius-like)
     * @param color Point color
     * @example
     * Shapes::DrawPoint(100, 100, 3.0f, Color::White());  // 3px point
     */
    void DrawPoint(float x, float y, float size, const Color& color);

} // namespace Shapes

} // namespace RS
