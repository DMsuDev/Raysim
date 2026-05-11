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

#include "Raysim/Math/Types/Vector2.hpp"

#include <cstdint>

namespace RS::Fonts
{
    // Normalized UV coordinates in the atlas [0,1], for GPU rendering.
    struct UVRect
    {
        Math::Vec2f uvMin; // Top-left
        Math::Vec2f uvMax; // Bottom-right
    };

    // Pixel region inside the atlas.
    struct AtlasRegion
    {
        Math::Vec2u16 atlasPos;  // Top-left (pixels)
        Math::Vec2u16 atlasSize; // Width/height (pixels)
    };

    // Glyph metrics and atlas placement.
    struct Glyph
    {
        uint32_t codepoint = 0; // Unicode codepoint

        AtlasRegion atlasRegion; // Pixel region in the atlas
        Math::Vec2f offset;       // Offset from baseline to top-left (pixels)
        float advanceX = 0.0f;   // Cursor advance (pixels)

        UVRect uvRect; // Normalized UVs for rendering
    };

} // namespace RS::Fonts
