#include "RaySim/Graphics/Color.hpp"
#include "pch.hpp"

RS::Color RS::Color::operator+(const RS::Color& o) const {
    return {
        static_cast<unsigned char>(std::min(255, r + o.r)),
        static_cast<unsigned char>(std::min(255, g + o.g)),
        static_cast<unsigned char>(std::min(255, b + o.b)),
        static_cast<unsigned char>(std::min(255, a + o.a))
    };
}

RS::Color RS::Color::lerp(const RS::Color& a, const RS::Color& b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return {
        static_cast<unsigned char>(a.r + (b.r - a.r) * t),
        static_cast<unsigned char>(a.g + (b.g - a.g) * t),
        static_cast<unsigned char>(a.b + (b.b - a.b) * t),
        static_cast<unsigned char>(a.a + (b.a - a.a) * t)
    };
}
