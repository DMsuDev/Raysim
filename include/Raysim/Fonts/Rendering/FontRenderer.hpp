#pragma once

#include "Raysim/Fonts/Types/Font.hpp"
#include "Raysim/Fonts/Types/FontTypes.hpp"

#include "Raysim/Graphics/Color.hpp"
#include "Raysim/Math/Types/Vector2.hpp"

#include <string>

namespace RS::Fonts {

class FontRenderer {
public:
    virtual ~FontRenderer() = default;

// =========================================
// Lifecycle
// =========================================

    // Called once after construction. Implementations may create GPU resources here.
    virtual void Init() {}

    // Called on shutdown. Release all GPU resources.
    virtual void Shutdown() {}

// =========================================
// Atlas management
// =========================================

    /**
     * @brief Upload a CPU atlas bitmap to a GPU texture.
     *
     * The R8 grayscale bitmap is converted to RGBA (white + alpha) internally
     * so that color tinting works correctly at render time.
     *
     * @param handle  Handle of the font this atlas belongs to.
     * @param atlas   Source CPU atlas (must be valid).
     * @return true on success, false if the GPU upload failed.
     */
    virtual bool UploadFontAtlas(FontHandle handle, const FontAtlas& atlas) = 0;

    /**
     * @brief Release the GPU texture for the given handle.
     * Safe to call even if no texture was uploaded for this handle.
     */
    virtual void ReleaseFontAtlas(FontHandle handle) = 0;

// =========================================
// Drawing
// =========================================

    /**
     * @brief Draw a text string to the screen.
     *
     * @param handle    Font handle (must match a previously uploaded atlas,
     *                  or DEFAULT_FONT_HANDLE for the engine built-in fallback).
     * @param font      Font struct (for glyph metrics, ascender, etc.).
     * @param text      UTF-8 string to render.
     * @param x         Screen X of the top-left corner of the first character.
     * @param y         Screen Y of the top-left corner of the first character.
     * @param fontSize  Render size in pixels (may differ from atlas rasterization size).
     * @param spacing   Extra pixels between characters.
     * @param color     Text tint color.
     */
    virtual void RenderText(
        FontHandle         handle,
        const Font&        font,
        const std::string& text,
        float              x,
        float              y,
        float              fontSize,
        float              spacing,
        const Color&       color) = 0;

    /**
     * @brief Measure the size of a text string rendered with the built-in default font.
     *
     * Used as a fallback when no TTF font is loaded.
     * @param text      Text to measure.
     * @param fontSize  Font size in pixels.
     * @return Bounding box as Vec2{width, height}.
     */
    virtual Math::Vec2f MeasureDefaultText(const std::string& text, float fontSize) const
    {
        // Conservative approximation: ~0.5 * fontSize per char
        return { static_cast<float>(text.size()) * fontSize * 0.5f, fontSize };
    }
};

} // namespace RS::Fonts
