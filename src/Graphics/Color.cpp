#include "../pch.hpp"

#include "Raysim/Graphics/Color.hpp"

#include <raylib.h>

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
