#include "rspch.hpp"

#include "Raysim/Graphics/Texts.hpp"
#include "Raysim/Fonts/FontManager.hpp"



namespace RS {

namespace {
    using namespace RS::Fonts;

    static void DrawTextInternal(const std::string& text, float x, float y, float fontSize, float spacing, const Color& color)
    {
        FontHandle handle = FontManager::GetDefaultHandle();
        FontManager::RenderText(handle, text, x, y, fontSize, spacing, color);
    }

    static Math::Vec2f MeasureTextDimensions(const std::string& text, float fontSize, float spacing)
    {
        FontHandle handle = FontManager::GetDefaultHandle();
        Math::Vec2f dims = FontManager::MeasureText(handle, text, spacing);
        // Scale measured result to the requested fontSize
        const Font* font = FontManager::GetFont(handle);
        float scale = (font && font->GetSize() > 0) ? (fontSize / static_cast<float>(font->GetSize())) : 1.0f;
        return { dims.x * scale, dims.y * scale };
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

    DrawTextInternal(text, x, y, fontSize, spacing, color);
}

//==============================================================================
// Text with origin mode
//==============================================================================

void Text::RenderText(const std::string& text, float x, float y, int fontSize, const Color& color, OriginMode origin) {
    RS_ASSERT(!text.empty(), "Text cannot be empty");
    RS_ASSERT(fontSize > 0, "Font size must be positive: {}", fontSize);

    Math::Vec2f dims = MeasureTextDimensions(text, static_cast<float>(fontSize), 0.0f);
    Math::Vec2f originVec = OriginToVector(origin);
    float adjustedX = x - (originVec.x * dims.x);
    float adjustedY = y - (originVec.y * dims.y);

    DrawTextInternal(text, adjustedX, adjustedY, static_cast<float>(fontSize), 0.0f, color);
}

void Text::RenderTextEx(const std::string& text, float x, float y, float fontSize, float spacing, const Color& color, OriginMode origin) {
    RS_ASSERT(!text.empty(), "Text cannot be empty");
    RS_ASSERT(fontSize > 0, "Font size must be positive: {}", fontSize);

    Math::Vec2f dims = MeasureTextDimensions(text, fontSize, spacing);
    Math::Vec2f originVec = OriginToVector(origin);
    float adjustedX = x - (originVec.x * dims.x);
    float adjustedY = y - (originVec.y * dims.y);

    DrawTextInternal(text, adjustedX, adjustedY, fontSize, spacing, color);
}

} // namespace RS
