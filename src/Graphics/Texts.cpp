#include "../pch.hpp"

#include "Raysim/Graphics/Texts.hpp"
#include "Raysim/Manager/FontManager.hpp"

#include <raylib.h>

namespace RS {

// Helper: convert RS::Color to raylib Color
static ::Color ToRL(const Color& c) {
    return ::Color{c.r, c.g, c.b, c.a};
}

// Helper: convert OriginMode to normalized origin vector (0-1 range)
static ::Vector2 OriginToVector(OriginMode origin) {
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
    }

    return {x, y};
}

// ============================================================================
// TEXT DRAWING - BASIC (NO ORIGIN MODE)
// ============================================================================

void Text::DrawText(const std::string& text, float x, float y, int fontSize, const Color& color) {
    RS_ASSERT(fontSize > 0, "Font size must be positive: {}", fontSize);
    if (RS::FontManager::HasFont()) {
        ::Font* fontPtr = static_cast<::Font*>(RS::FontManager::GetFont().data);
        RS_ASSERT_NOT_NULL(fontPtr);
        ::DrawTextEx(*fontPtr, text.c_str(), {x, y}, static_cast<float>(fontSize), 0, ToRL(color));
    } else {
        // Fallback to default raylib font
        ::DrawText(text.c_str(), static_cast<int>(x), static_cast<int>(y), static_cast<int>(fontSize), ToRL(color));
    }
}

void Text::DrawTextEx(const std::string& text, float x, float y, float fontSize, float spacing, const Color& color) {
    RS_ASSERT(fontSize > 0, "Font size must be positive: {}", fontSize);
    RS_ASSERT(spacing >= 0, "Spacing must be non-negative: {}", spacing);
    // Use loaded font if available, otherwise fall back to raylib default
    if (RS::FontManager::HasFont()) {
        ::Font* fontPtr = static_cast<::Font*>(RS::FontManager::GetFont().data);
        RS_ASSERT_NOT_NULL(fontPtr);
        ::DrawTextEx(*fontPtr, text.c_str(), {x, y}, fontSize, spacing, ToRL(color));
    } else {
        // Fallback to default raylib font
        ::DrawText(text.c_str(), static_cast<int>(x), static_cast<int>(y), static_cast<int>(fontSize), ToRL(color));
    }
}

// ============================================================================
// TEXT DRAWING - WITH ORIGIN MODE
// ============================================================================

void Text::DrawText(const std::string& text, float x, float y, int fontSize, const Color& color, OriginMode origin) {
    // Measure text dimensions accurately using raylib when possible
    float textWidth = 0.0f;
    float textHeight = 0.0f;

    if (RS::FontManager::HasFont()) {
        ::Font* fontPtr = static_cast<::Font*>(RS::FontManager::GetFont().data);
        RS_ASSERT_NOT_NULL(fontPtr);
        ::Vector2 m = ::MeasureTextEx(*fontPtr, text.c_str(), static_cast<float>(fontSize), 0.0f);
        textWidth = m.x;
        textHeight = m.y;
    } else {
        // Fallback to raylib default font measurement
        textWidth = static_cast<float>(::MeasureText(text.c_str(), static_cast<int>(fontSize)));
        textHeight = static_cast<float>(fontSize);
    }

    ::Vector2 originVec = OriginToVector(origin);
    float adjustedX = x - (originVec.x * textWidth);
    float adjustedY = y - (originVec.y * textHeight);

    if (RS::FontManager::HasFont()) {
        ::Font* fontPtr = static_cast<::Font*>(RS::FontManager::GetFont().data);
        RS_ASSERT_NOT_NULL(fontPtr);
        ::DrawTextEx(*fontPtr, text.c_str(), {adjustedX, adjustedY}, static_cast<float>(fontSize), 0, ToRL(color));
    } else {
        ::DrawText(text.c_str(), static_cast<int>(adjustedX), static_cast<int>(adjustedY), static_cast<int>(fontSize), ToRL(color));
    }
}

void Text::DrawTextEx(const std::string& text, float x, float y, float fontSize, float spacing, const Color& color, OriginMode origin) {
    // Measure text dimensions accurately using raylib when possible
    float textWidth = 0.0f;
    float textHeight = 0.0f;

    if (RS::FontManager::HasFont()) {
        ::Font* fontPtr = static_cast<::Font*>(RS::FontManager::GetFont().data);
        RS_ASSERT_NOT_NULL(fontPtr);
        ::Vector2 m = ::MeasureTextEx(*fontPtr, text.c_str(), fontSize, spacing);
        textWidth = m.x;
        textHeight = m.y;
    } else {
        // Fallback to raylib default font measurement
        textWidth = static_cast<float>(::MeasureText(text.c_str(), static_cast<int>(fontSize)));
        textHeight = fontSize;
    }

    ::Vector2 originVec = OriginToVector(origin);
    float adjustedX = x - (originVec.x * textWidth);
    float adjustedY = y - (originVec.y * textHeight);

    if (RS::FontManager::HasFont()) {
        ::Font* fontPtr = static_cast<::Font*>(RS::FontManager::GetFont().data);
        RS_ASSERT_NOT_NULL(fontPtr);
        ::DrawTextEx(*fontPtr, text.c_str(), {adjustedX, adjustedY}, fontSize, spacing, ToRL(color));
    } else {
        ::DrawText(text.c_str(), static_cast<int>(adjustedX), static_cast<int>(adjustedY), static_cast<int>(fontSize), ToRL(color));
    }
}

// ============================================================================
// TEXT IN RECTANGLE
// ============================================================================

void Text::DrawTextInRect(const std::string& text, float x, float y, float w, float h, float fontSize, const Color& color, OriginMode origin, float spacing) {
    // Measure text dimensions
    float textWidth = 0.0f;
    float textHeight = 0.0f;

    if (RS::FontManager::HasFont()) {
        ::Font* fontPtr = static_cast<::Font*>(RS::FontManager::GetFont().data);
        RS_ASSERT_NOT_NULL(fontPtr);
        ::Vector2 m = ::MeasureTextEx(*fontPtr, text.c_str(), fontSize, spacing);
        textWidth = m.x;
        textHeight = m.y;
    } else {
        textWidth = static_cast<float>(::MeasureText(text.c_str(), static_cast<int>(fontSize)));
        textHeight = fontSize;
    }

    ::Vector2 originVec = OriginToVector(origin);

    // Compute top-left position where to draw text so it's aligned inside rect
    float drawX = x + (w - textWidth) * originVec.x;
    float drawY = y + (h - textHeight) * originVec.y;

    if (RS::FontManager::HasFont()) {
        ::Font* fontPtr = static_cast<::Font*>(RS::FontManager::GetFont().data);
        RS_ASSERT_NOT_NULL(fontPtr);
        ::DrawTextEx(*fontPtr, text.c_str(), {drawX, drawY}, fontSize, spacing, ToRL(color));
    } else {
        ::DrawText(text.c_str(), static_cast<int>(drawX), static_cast<int>(drawY), static_cast<int>(fontSize), ToRL(color));
    }
}

} // namespace RS
