#pragma once

#include "Raysim/Fonts/Types/Font.hpp"
#include "Raysim/Math/Types/Vector2.hpp"

#include <string>
#include <cstdint>

namespace RS::Fonts {

class TrueTypeProvider {
public:
    virtual ~TrueTypeProvider() = default;

    /**
     * @brief Load a TTF/OTF font from disk and rasterize a glyph range.
     *
     * @param path       Filesystem path to the TTF/OTF file.
     * @param fontSize   Desired rasterization size in pixels (e.g. 32).
     * @param firstChar  First Unicode codepoint to include in the atlas (default 32 = space).
     * @param charCount  Number of consecutive codepoints to rasterize (default 95 = printable ASCII).
     * @param outFont    Output: populated Font struct on success.
     * @return true on success, false if the file is missing or malformed.
     */
    virtual bool LoadFont(
        const std::string& path,
        int                fontSize,
        uint32_t           firstChar,
        uint32_t           charCount,
        Font&              outFont) = 0;

    /**
     * @brief Measure the pixel dimensions of a text string at native font size.
     *
     * Returns the tight bounding box of the string as rendered by this font.
     * Height is always font.atlas.lineHeight (single line only).
     *
     * @param font    Previously loaded Font.
     * @param text    UTF-8 string to measure.
     * @param spacing Extra pixels added between each character (default 0).
     * @return Pixel dimensions as Vec2{width, height}.
     */
    virtual Math::Vec2f MeasureText(
        const Font&        font,
        const std::string& text,
        float              spacing = 0.0f) const = 0;
};

} // namespace RS::Fonts
