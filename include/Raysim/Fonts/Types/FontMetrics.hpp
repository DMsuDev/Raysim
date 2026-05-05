#pragma once

namespace RS::Fonts {

// Glyph metrics and atlas placement.
struct FontMetrics {
    float ascent  = 0.0f;
    float descent = 0.0f;
    float lineGap = 0.0f;

    // Recommended line height (ascent - descent + lineGap)
    float lineHeight = 0.0f;
};

} // namespace RS::Fonts
