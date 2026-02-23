#include "Raysim/Graphics/Color.hpp"
#include "pch.hpp"

#include <raylib.h>

RS::RColor RS::RColor::operator+(const RS::RColor& o) const {
    return {
        static_cast<unsigned char>(std::min(255, r + o.r)),
        static_cast<unsigned char>(std::min(255, g + o.g)),
        static_cast<unsigned char>(std::min(255, b + o.b)),
        static_cast<unsigned char>(std::min(255, a + o.a))
    };
}

RS::RColor RS::RColor::lerp(const RS::RColor& a, const RS::RColor& b, float t) {
    t = std::clamp(t, 0.0f, 1.0f);
    return {
        static_cast<unsigned char>(a.r + (b.r - a.r) * t),
        static_cast<unsigned char>(a.g + (b.g - a.g) * t),
        static_cast<unsigned char>(a.b + (b.b - a.b) * t),
        static_cast<unsigned char>(a.a + (b.a - a.a) * t)
    };
}

::Color RS::RColor::raylib() const {
    return ::Color{r, g, b, a};
}
