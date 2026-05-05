#pragma once

#include <cstdint>

namespace RS::Fonts {

// --- Font Handle ---

using FontHandle = uint32_t;

// Sentinel value representing an invalid or failed font load.
constexpr FontHandle INVALID_FONT_HANDLE = 0;

// Special handle used internally for the engine default font (backend built-in).
constexpr FontHandle DEFAULT_FONT_HANDLE = 1;

// --- Glyph Index ---

using GlyphIndex = uint32_t;

// Sentinel value representing an invalid glyph index (e.g. for unmapped codepoints).
constexpr GlyphIndex INVALID_GLYPH = 0xFFFFFFFF;

} // namespace RS::Fonts
