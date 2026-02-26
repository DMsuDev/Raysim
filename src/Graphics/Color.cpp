#include "../pch.hpp"

#include "Raysim/Graphics/Color.hpp"

#include <raylib.h>

// =======================================================
// COLOR OPERATIONS
// =======================================================

RS::Color RS::Color::Lerp(const RS::Color& a, const RS::Color& b, float t) {
    RS_ASSERT(t >= 0.0f && t <= 1.0f, "Lerp parameter must be in range [0, 1], got: {}", t);
    t = std::clamp(t, 0.0f, 1.0f);
    return {
        static_cast<unsigned char>(a.r + (b.r - a.r) * t),
        static_cast<unsigned char>(a.g + (b.g - a.g) * t),
        static_cast<unsigned char>(a.b + (b.b - a.b) * t),
        static_cast<unsigned char>(a.a + (b.a - a.a) * t)
    };
}

// =======================================================
// FACTORY METHODS
// =======================================================

RS::Color RS::Color::FromHex(const std::string& hex) {
    RS_ASSERT(hex.length() == 7 || hex.length() == 9, "Hex color must be in format '#RRGGBB' or '#RRGGBBAA', got: '{}'", hex);
    RS_ASSERT(hex[0] == '#', "Hex color must start with '#', got: '{}'", hex);

    try
    {
        unsigned char r = std::stoul(hex.substr(1, 2), nullptr, 16);
        unsigned char g = std::stoul(hex.substr(3, 2), nullptr, 16);
        unsigned char b = std::stoul(hex.substr(5, 2), nullptr, 16);
        unsigned char a = (hex.length() == 9) ? std::stoul(hex.substr(7, 2), nullptr, 16) : 255;

        return Color{r, g, b, a};
    }
    catch (const std::exception& e)
    {
        LOG_ERROR("Error parsing hex color '{}': {}", hex, e.what());
        return Color{};
    }
}

RS::Color RS::Color::FromHSV(float h, float s, float v, unsigned char alpha) {
    h = std::fmod(h, 360.0f);
    if (h < 0) h += 360.0f;

    s = std::clamp(s, 0.0f, 1.0f);
    v = std::clamp(v, 0.0f, 1.0f);

    float c = v * s; // Chroma
    float x = c * (1.0f - std::fabs(std::fmod(h / 60.0f, 2) - 1));
    float m = v - c;

    float values[3] = {c, x, 0};

    // Pattern for RGB assignment based on hue sector
    const int pattern[6][3] = {
        {0, 1, 2}, // 0–60
        {1, 0, 2}, // 60–120
        {2, 0, 1}, // 120–180
        {2, 1, 0}, // 180–240
        {1, 2, 0}, // 240–300
        {0, 2, 1}  // 300–360
    };

    // Determine which sector of the hue circle we're in
    // Using integer division / 60 to get the sector index, and modulo 6 to wrap around
    int sector = static_cast<int>(h / 60.0f) % 6;

    float r = values[pattern[sector][0]] + m;
    float g = values[pattern[sector][1]] + m;
    float b = values[pattern[sector][2]] + m;

    return {
        static_cast<unsigned char>(r * 255),
        static_cast<unsigned char>(g * 255),
        static_cast<unsigned char>(b * 255),
        alpha
    };
}

// =======================================================
// CONVERSION UTILITIES
// =======================================================

std::string RS::Color::ToHex(bool includeAlpha) const {
    static constexpr char hex[] = "0123456789ABCDEF";
    char buffer[9];
    buffer[0] = '#';
    buffer[1] = hex[r >> 4]; buffer[2] = hex[r & 0xF];
    buffer[3] = hex[g >> 4]; buffer[4] = hex[g & 0xF];
    buffer[5] = hex[b >> 4]; buffer[6] = hex[b & 0xF];
    int len = 7;
    if (includeAlpha) {
        buffer[7] = hex[a >> 4]; buffer[8] = hex[a & 0xF];
        len = 9;
    }
    return std::string(buffer, len);
}

RS::Vector3 RS::Color::ToHSV() const
{
    // Normalize RGB values to [0, 1]
    float r = this->r * (1.0f / 255.0f);
    float g = this->g * (1.0f / 255.0f);
    float b = this->b * (1.0f / 255.0f);

    float maxc = std::max({r, g, b});
    float minc = std::min({r, g, b});
    float delta = maxc - minc;

    float h = 0.0f;
    float s = (maxc > 0.0f) ? delta / maxc : 0.0f;
    float v = maxc;

    if (delta > 0.0f)
    {
        if (maxc == r)
            h = (g - b) / delta;
        else if (maxc == g)
            h = 2.0f + (b - r) / delta;
        else
            h = 4.0f + (r - g) / delta;

        h *= 60.0f;

        if (h < 0.0f) h += 360.0f;
    }

    return {h, s, v};
}

// =======================================================
// OPERATORS
// =======================================================

RS::Color RS::Color::operator+(const RS::Color& o) const {
    return {
        static_cast<unsigned char>(std::min(255, r + o.r)),
        static_cast<unsigned char>(std::min(255, g + o.g)),
        static_cast<unsigned char>(std::min(255, b + o.b)),
        static_cast<unsigned char>(std::min(255, a + o.a))
    };
}

RS::Color RS::Color::operator-(const RS::Color& rhs) const {
    return {
        static_cast<unsigned char>(std::max(0, r - rhs.r)),
        static_cast<unsigned char>(std::max(0, g - rhs.g)),
        static_cast<unsigned char>(std::max(0, b - rhs.b)),
        static_cast<unsigned char>(std::max(0, a - rhs.a))
    };
}
