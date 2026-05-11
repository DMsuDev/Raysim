/**********************************************************************************************
 *   Raysim - A C++ framework for 2D graphics and interactive applications
 *
 *   LICENSE: Apache License, Version 2.0
 *
 *            Copyright 2026 Dayron Mustelier (@DMsuDev)
 *
 *   Raysim is licensed under the Apache License, Version 2.0 (the "License");
 *   you may not use this file except in compliance with the License.
 *   You may obtain a copy of the License at
 *
 *             http://www.apache.org/licenses/LICENSE-2.0
 *
 *   Unless required by applicable law or agreed to in writing, software
 *   distributed under the License is distributed on an "AS IS" BASIS,
 *   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *   See the License for the specific language governing permissions and
 *   limitations under the License.
 *
 **********************************************************************************************/

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
