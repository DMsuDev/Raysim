#include "../pch.hpp"

#include "Raysim/Graphics/Color.hpp"

#include <raylib.h>

RS::Color RS::Color::operator+(const RS::Color& o) const {
    return {
        RS::Cast<unsigned char>(std::min(255, r + o.r)),
        RS::Cast<unsigned char>(std::min(255, g + o.g)),
        RS::Cast<unsigned char>(std::min(255, b + o.b)),
        RS::Cast<unsigned char>(std::min(255, a + o.a))
    };
}

RS::Color RS::Color::operator-(const RS::Color& rhs) const {
    return {
        RS::Cast<unsigned char>(std::max(0, r - rhs.r)),
        RS::Cast<unsigned char>(std::max(0, g - rhs.g)),
        RS::Cast<unsigned char>(std::max(0, b - rhs.b)),
        RS::Cast<unsigned char>(std::max(0, a - rhs.a))
    };
}

RS::Color RS::Color::Lerp(const RS::Color& a, const RS::Color& b, float t) {
    RS_ASSERT(t >= 0.0f && t <= 1.0f, "Lerp parameter must be in range [0, 1], got: {}", t);
    t = std::clamp(t, 0.0f, 1.0f);
    return {
        RS::Cast<unsigned char>(a.r + (b.r - a.r) * t),
        RS::Cast<unsigned char>(a.g + (b.g - a.g) * t),
        RS::Cast<unsigned char>(a.b + (b.b - a.b) * t),
        RS::Cast<unsigned char>(a.a + (b.a - a.a) * t)
    };
}
