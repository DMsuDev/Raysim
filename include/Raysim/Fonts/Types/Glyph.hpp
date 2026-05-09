#pragma once

#include "Raysim/Math/Types/Vec2.hpp"

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
