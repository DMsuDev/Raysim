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
