#include "pch.hpp"

#include "Raysim/Graphics/Texts.hpp"
#include "Raysim/Core/FontManager.hpp"

#include <raylib.h>

namespace RS {

namespace {
    /// Convert `RS::Color` to raylib's `Color`.
    [[nodiscard]] constexpr ::Color ToRL(const Color& c) noexcept
    {
        return ::Color{ c.r, c.g, c.b, c.a };
    }

    /// Convert `OriginMode` to a normalized origin vector in [0..1].
    [[nodiscard]] constexpr ::Vector2 OriginToVector(OriginMode origin) noexcept
    {
        float x = 0.0f, y = 0.0f;

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

    static void DrawTextInternal(const std::string& text, float x, float y, float fontSize, float spacing, const Color& color)
    {
        if (RS::FontManager::HasFont()) {
            ::Font* fontPtr = static_cast<::Font*>(RS::FontManager::GetFont().data);
            RS_ASSERT_NOT_NULL(fontPtr);
            ::DrawTextEx(*fontPtr, text.c_str(), {x, y}, fontSize, spacing, ToRL(color));
        } else {
            ::DrawText(text.c_str(), static_cast<int>(x), static_cast<int>(y), static_cast<int>(fontSize), ToRL(color));
        }
    }

    static ::Vector2 MeasureTextDimensions(const std::string& text, float fontSize, float spacing)
    {
        if (RS::FontManager::HasFont()) {
            ::Font* fontPtr = static_cast<::Font*>(RS::FontManager::GetFont().data);
            RS_ASSERT_NOT_NULL(fontPtr);
            return ::MeasureTextEx(*fontPtr, text.c_str(), fontSize, spacing);
        } else {
            float width = static_cast<float>(::MeasureText(text.c_str(), static_cast<int>(fontSize)));
            return {width, fontSize};
        }
    }
} // namespace

//==============================================================================
// Basic text (no origin mode)
//==============================================================================

void Text::RenderText(const std::string& text, float x, float y, int fontSize, const Color& color) {
    RS_ASSERT(!text.empty(), "Text cannot be empty");
    RS_ASSERT(fontSize > 0, "Font size must be positive: {}", fontSize);

    DrawTextInternal(text, x, y, static_cast<float>(fontSize), 0.0f, color);
}

void Text::RenderTextEx(const std::string& text, float x, float y, float fontSize, float spacing, const Color& color) {
    RS_ASSERT(!text.empty(), "Text cannot be empty");
    RS_ASSERT(fontSize > 0, "Font size must be positive: {}", fontSize);
    RS_ASSERT(spacing >= 0, "Spacing must be non-negative: {}", spacing);

    DrawTextInternal(text, x, y, fontSize, spacing, color);
}

//==============================================================================
// Text with origin mode
//==============================================================================

void Text::RenderText(const std::string& text, float x, float y, int fontSize, const Color& color, OriginMode origin) {
    RS_ASSERT(!text.empty(), "Text cannot be empty");
    RS_ASSERT(fontSize > 0, "Font size must be positive: {}", fontSize);

    ::Vector2 dims = MeasureTextDimensions(text, static_cast<float>(fontSize), 0.0f);
    ::Vector2 originVec = OriginToVector(origin);
    float adjustedX = x - (originVec.x * dims.x);
    float adjustedY = y - (originVec.y * dims.y);

    DrawTextInternal(text, adjustedX, adjustedY, static_cast<float>(fontSize), 0.0f, color);
}

void Text::RenderTextEx(const std::string& text, float x, float y, float fontSize, float spacing, const Color& color, OriginMode origin) {
    RS_ASSERT(!text.empty(), "Text cannot be empty");
    RS_ASSERT(fontSize > 0, "Font size must be positive: {}", fontSize);
    RS_ASSERT(spacing >= 0, "Spacing must be non-negative: {}", spacing);

    ::Vector2 dims = MeasureTextDimensions(text, fontSize, spacing);
    ::Vector2 originVec = OriginToVector(origin);
    float adjustedX = x - (originVec.x * dims.x);
    float adjustedY = y - (originVec.y * dims.y);

    DrawTextInternal(text, adjustedX, adjustedY, fontSize, spacing, color);
}

//==============================================================================
// Text in rectangle
//==============================================================================

void Text::RenderTextInRectangle(const std::string& text, float x, float y, float w, float h, float fontSize, const Color& color, OriginMode origin, float spacing) {
    RS_ASSERT(!text.empty(), "Text cannot be empty");
    RS_ASSERT(fontSize > 0, "Font size must be positive: {}", fontSize);
    RS_ASSERT(spacing >= 0, "Spacing must be non-negative: {}", spacing);
    RS_ASSERT(w > 0, "Rectangle width must be positive: {}", w);
    RS_ASSERT(h > 0, "Rectangle height must be positive: {}", h);

    ::Vector2 dims = MeasureTextDimensions(text, fontSize, spacing);
    ::Vector2 originVec = OriginToVector(origin);

    float drawX = x + (w - dims.x) * originVec.x;
    float drawY = y + (h - dims.y) * originVec.y;

    DrawTextInternal(text, drawX, drawY, fontSize, spacing, color);
}

} // namespace RS
