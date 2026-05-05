#pragma once

#include <cstdint>
#include <ostream>

namespace RS::Math {

/**
 * @struct Vec2u16
 * @brief 2D vector with unsigned 16-bit integer components.
 *
 * Designed for compact pixel-space data such as atlas bounding boxes,
 * where values are always non-negative and fit within [0, 65535].
 */
struct Vec2u16 {
    uint16_t x{0}, y{0};

    constexpr Vec2u16() noexcept = default;
    constexpr Vec2u16(uint16_t in_x, uint16_t in_y) noexcept : x(in_x), y(in_y) {}
    explicit constexpr Vec2u16(uint16_t s) noexcept : x(s), y(s) {}

//==============================================================================
// Operators
//==============================================================================

    constexpr Vec2u16 operator+(const Vec2u16& o) const noexcept { return {static_cast<uint16_t>(x + o.x), static_cast<uint16_t>(y + o.y)}; }
    constexpr Vec2u16 operator-(const Vec2u16& o) const noexcept { return {static_cast<uint16_t>(x - o.x), static_cast<uint16_t>(y - o.y)}; }
    constexpr Vec2u16 operator*(uint16_t s)          const noexcept { return {static_cast<uint16_t>(x * s),   static_cast<uint16_t>(y * s)}; }
    constexpr Vec2u16 operator/(uint16_t s)          const noexcept { return (s != 0) ? Vec2u16{static_cast<uint16_t>(x / s), static_cast<uint16_t>(y / s)} : Vec2u16{}; }

    constexpr Vec2u16& operator+=(const Vec2u16& o) noexcept { x += o.x; y += o.y; return *this; }
    constexpr Vec2u16& operator-=(const Vec2u16& o) noexcept { x -= o.x; y -= o.y; return *this; }
    constexpr Vec2u16& operator*=(uint16_t s)          noexcept { x *= s; y *= s; return *this; }
    constexpr Vec2u16& operator/=(uint16_t s)          noexcept { if (s != 0) { x /= s; y /= s; } return *this; }

    constexpr bool operator==(const Vec2u16& o) const noexcept { return x == o.x && y == o.y; }
    constexpr bool operator!=(const Vec2u16& o) const noexcept { return !(*this == o); }

    constexpr uint16_t& operator[](size_t i) noexcept       { return (&x)[i]; }
    constexpr const uint16_t& operator[](size_t i) const noexcept { return (&x)[i]; }

//==============================================================================
// Math
//==============================================================================

    constexpr uint32_t LengthSquared() const noexcept
    {
        return static_cast<uint32_t>(x) * x + static_cast<uint32_t>(y) * y;
    }

//==============================================================================
// Common vectors
//==============================================================================

    static constexpr Vec2u16 Zero()  noexcept { return {0, 0}; }
    static constexpr Vec2u16 One()   noexcept { return {1, 1}; }
};

constexpr Vec2u16 operator*(uint16_t s, const Vec2u16& v) noexcept { return v * s; }

inline std::ostream& operator<<(std::ostream& os, const Vec2u16& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

} // namespace RS::Math
