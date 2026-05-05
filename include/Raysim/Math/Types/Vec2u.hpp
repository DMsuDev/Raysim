#pragma once

#include <cstdint>
#include <ostream>

namespace RS::Math {

/**
 * @struct Vec2u
 * @brief 2D vector with unsigned 32-bit integer components.
 *
 * Designed for pixel-space quantities that are always non-negative and
 * may exceed 16-bit range, such as texture dimensions or window sizes.
 */
struct Vec2u {
    uint32_t x{0}, y{0};

    constexpr Vec2u() noexcept = default;
    constexpr Vec2u(uint32_t in_x, uint32_t in_y) noexcept : x(in_x), y(in_y) {}
    explicit constexpr Vec2u(uint32_t s) noexcept : x(s), y(s) {}

//==============================================================================
// Operators
//==============================================================================

    constexpr Vec2u operator+(const Vec2u& o) const noexcept { return {x + o.x, y + o.y}; }
    constexpr Vec2u operator-(const Vec2u& o) const noexcept { return {x - o.x, y - o.y}; }
    constexpr Vec2u operator*(uint32_t s)         const noexcept { return {x * s, y * s}; }
    constexpr Vec2u operator/(uint32_t s)         const noexcept { return (s != 0u) ? Vec2u{x / s, y / s} : Vec2u{}; }

    constexpr Vec2u& operator+=(const Vec2u& o) noexcept { x += o.x; y += o.y; return *this; }
    constexpr Vec2u& operator-=(const Vec2u& o) noexcept { x -= o.x; y -= o.y; return *this; }
    constexpr Vec2u& operator*=(uint32_t s)         noexcept { x *= s; y *= s; return *this; }
    constexpr Vec2u& operator/=(uint32_t s)         noexcept { if (s != 0u) { x /= s; y /= s; } return *this; }

    constexpr bool operator==(const Vec2u& o) const noexcept { return x == o.x && y == o.y; }
    constexpr bool operator!=(const Vec2u& o) const noexcept { return !(*this == o); }

    constexpr uint32_t& operator[](size_t i) noexcept             { return (&x)[i]; }
    constexpr const uint32_t& operator[](size_t i) const noexcept { return (&x)[i]; }

//==============================================================================
// Math
//==============================================================================

    constexpr uint64_t LengthSquared() const noexcept
    {
        return static_cast<uint64_t>(x) * x + static_cast<uint64_t>(y) * y;
    }

//==============================================================================
// Common vectors
//==============================================================================

    static constexpr Vec2u Zero() noexcept { return {0u, 0u}; }
    static constexpr Vec2u One()  noexcept { return {1u, 1u}; }
};

constexpr Vec2u operator*(uint32_t s, const Vec2u& v) noexcept { return v * s; }

inline std::ostream& operator<<(std::ostream& os, const Vec2u& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

} // namespace RS::Math
