#pragma once

#include "Glyph.hpp"
#include "FontTypes.hpp"
#include "FontMetrics.hpp"

#include <cstdint>
#include <vector>
#include <unordered_map>

namespace RS::Fonts {

// GPU ready font atlas containing the rasterized glyph bitmaps and their metrics.
struct FontAtlas {

    // GPU texture ID for the atlas bitmap
    uint32_t textureID = 0;

    uint16_t width  = 0;
    uint16_t height = 0;

    // Vertical font metrics (at native render size)
    FontMetrics metrics;

    // R8 grayscale pixel data, row-major. Size == width * height.
    std::vector<uint8_t> data;

    // Storage for glyph metrics and atlas placement. Indexed by codepointToIndex.
    std::vector<Glyph> glyphs;

    // Lookup table mapping Unicode codepoints to indices in the glyphs vector.
    std::vector<uint32_t> codepointToIndex;

    // Returns true when the atlas has been populated with valid bitmap data.
    [[nodiscard]] bool IsValid() const noexcept {
        return width > 0 && height > 0 && !data.empty();
    }

    // Looks up the Glyph for a given Unicode codepoint. Returns nullptr if not found.
    [[nodiscard]] const Glyph* GetGlyph(uint32_t codepoint) const noexcept {
        if (codepoint >= codepointToIndex.size()) {
            return nullptr; // Codepoint out of range
        }
        uint32_t index = codepointToIndex[codepoint];
        if (index == INVALID_GLYPH) {
            return nullptr; // Codepoint not mapped to a glyph
        }
        return &glyphs[index];
    }
};

} // namespace RS::Fonts
