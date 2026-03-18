#pragma once

#include "Color.hpp"
#include "../Math/Vector2.hpp"
#include "OriginMode.hpp"

#include <string>

namespace RS {

/**
 * @namespace Text
 * @brief Text rendering API with flexible coordinate origin support
 *
 * Provides functions for drawing text with optional OriginMode parameter for
 * flexible coordinate systems. Without specifying origin, functions use TopLeft.
 *
 * All text functions support custom fonts via FontManager and automatic fallback
 * to raylib's built-in font when no custom font is loaded.
 *
 * @example
 * // Draw text centered at (400, 200)
 * Text::DrawText("Hello World", 400, 200, 24, Color::White(), OriginMode::Center);
 *
 * // Traditional top-left anchoring
 * Text::DrawText("Top-left text", 10, 10, 20, Color::Black());
 */
namespace Text {

//==============================================================================
// Basic text
//==============================================================================

    /**
     * @brief Draw text using a system-like font
     *
     * Renders text at specified coordinates with automatic font loading.
     * For better control, use DrawTextEx() which supports spacing.
     *
     * @param text The string to render
     * @param x X-coordinate for text placement
     * @param y Y-coordinate for text placement
     * @param fontSize Font size in pixels
     * @param color Text color
     * @see DrawText(const std::string&, float, float, int, const Color&, OriginMode)
     */
    void DrawText(const std::string& text, float x, float y, int fontSize, const Color& color);

    /**
     * @brief Draw text with custom origin point
     *
     * Allows anchoring text to different positions (top-left, center, bottom-right, etc.).
     * Useful for UI elements like centered headings or bottom-aligned descriptions.
     *
     * @param text The string to render
     * @param x X-coordinate of the anchor point
     * @param y Y-coordinate of the anchor point
     * @param fontSize Font size in pixels
     * @param color Text color
     * @param origin Which point of the text bounds is at (x, y)
     * @example
     * // Draw title centered at (400, 100)
     * Text::DrawText("Game Over", 400, 100, 28, Color::White(), OriginMode::TopCenter);
     *
     * // Draw credits at bottom-center
     * Text::DrawText("Copyright 2024", 400, 700, 12, Color::Gray(), OriginMode::BottomCenter);
     * @see OriginMode
     */
    void DrawText(const std::string& text, float x, float y, int fontSize, const Color& color, OriginMode origin);

//==============================================================================
// Extended text
//==============================================================================

    /**
     * @brief Draw text with custom spacing between characters
     *
     * Advanced text rendering with control over letter spacing.
     * Allows for stylized text rendering suitable for titles, menus, and UI.
     *
     * @param text The string to render
     * @param x X-coordinate for text placement
     * @param y Y-coordinate for text placement
     * @param fontSize Font size in pixels (floating-point for fine control)
     * @param spacing Horizontal space between characters in pixels (negative = tight)
     * @param color Text color
     * @example
     * Text::DrawTextEx("SPACED OUT", 200, 300, 20.0f, 2.0f, Color::Yellow());
     * @see DrawText(), DrawTextEx(const std::string&, float, float, float, float, const Color&, OriginMode)
     */
    void DrawTextEx(const std::string& text, float x, float y, float fontSize, float spacing, const Color& color);

    /**
     * @brief Draw text with custom spacing and origin point
     *
     * Combines character spacing control with flexible text anchoring.
     *
     * @param text The string to render
     * @param x X-coordinate of the anchor point
     * @param y Y-coordinate of the anchor point
     * @param fontSize Font size in pixels (floating-point)
     * @param spacing Space between characters in pixels
     * @param color Text color
     * @param origin Which point of the text bounds is at (x, y)
     * @example
     * // Draw "MENU" centered at (400, 200) with extra letter spacing
     * Text::DrawTextEx("MENU", 400, 200, 24.0f, 1.5f, Color::Gold(), OriginMode::Center);
     * @see OriginMode
     */
    void DrawTextEx(const std::string& text, float x, float y, float fontSize, float spacing, const Color& color, OriginMode origin);

//==============================================================================
// Layout helpers
//==============================================================================

    /**
     * @brief Draw text positioned inside a rectangle with alignment
     *
     * Positions the text within the rectangle defined by (x, y, w, h).
     * The text is aligned according to @c origin (use Center to center it).
     * If text is larger than the rect, it will be positioned according to the
     * chosen alignment and may overflow the rectangle bounds.
     *
     * @param text Text to draw
     * @param x Rectangle top-left X
     * @param y Rectangle top-left Y
     * @param w Rectangle width
     * @param h Rectangle height
     * @param fontSize Font size in pixels
     * @param color Text color
     * @param origin Alignment/anchor inside the rectangle (default: Center)
     * @param spacing Character spacing for DrawTextEx (default: 0.0f)
     * @example
     * // Draw "Click Me" centered in a 200x50 button
     * Text::DrawTextInRect("Click Me", 100, 200, 200, 50, 20, Color::White());
     */
    void DrawTextInRect(const std::string& text, float x, float y, float w, float h, float fontSize, const Color& color, OriginMode origin = OriginMode::Center, float spacing = 0.0f);

} // namespace Text

} // namespace RS
