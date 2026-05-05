#pragma once

#include "../Utils/MathUtils.hpp"

#include <cstdint>
#include <ostream>

namespace RS::Math {

/**
 * @struct Vec2i
 * @brief 2D vector with arithmetic and geometric operations
 *
 * Represents a 2D vector with integer components for positions, velocities,
 * and directions. Includes common operations like dot product, normalization, and projection.
 */
struct Vec2i {
    int x{0}, y{0};

    /**
     * @brief Default construct (zero vector)
     */
    constexpr Vec2i() noexcept = default;
    /**
     * @brief Construct from x and y components
     * @param in_x X component
     * @param in_y Y component
     */
    constexpr Vec2i(int in_x, int in_y) noexcept : x(in_x), y(in_y) {}
    /**
     * @brief Construct uniform vector (x = y = s)
     * @param s Scalar applied to both components
     */
    explicit constexpr Vec2i(int s) noexcept : x(s), y(s) {}

//==============================================================================
// Operators
//==============================================================================

    constexpr Vec2i operator+(const Vec2i& o) const noexcept { return {x + o.x, y + o.y}; }
    constexpr Vec2i operator-(const Vec2i& o) const noexcept { return {x - o.x, y - o.y}; }
    constexpr Vec2i operator*(int s)               const noexcept { return {x * s, y * s}; }
    constexpr Vec2i operator/(int s)               const noexcept { return (s != 0) ? Vec2i{x / s, y / s} : Vec2i{}; }

    constexpr Vec2i operator-() const noexcept { return {-x, -y}; }

    constexpr Vec2i& operator+=(const Vec2i& o) noexcept { x += o.x; y += o.y; return *this; }
    constexpr Vec2i& operator-=(const Vec2i& o) noexcept { x -= o.x; y -= o.y; return *this; }
    constexpr Vec2i& operator*=(int s)               noexcept { x *= s; y *= s; return *this; }
    constexpr Vec2i& operator/=(int s)               noexcept { if (s != 0) { x /= s; y /= s; } return *this; }

    constexpr bool operator==(const Vec2i& o) const noexcept { return x == o.x && y == o.y; }
    constexpr bool operator!=(const Vec2i& o) const noexcept { return !(*this == o); }

    constexpr int& operator[](size_t i) noexcept{ return (&x)[i]; }
    constexpr const int& operator[](size_t i) const noexcept{ return (&x)[i]; }

//==============================================================================
// Math
//==============================================================================

    /**
     * @brief Calculate vector magnitude (length)
     * @return Euclidean length of the vector
     */
    float Length() const noexcept { return std::sqrt(static_cast<float>(LengthSquared())); }
    /**
     * @brief Calculate squared magnitude (faster, avoids sqrt)
     * @return Squared length of the vector
     */
    constexpr int64_t LengthSquared() const noexcept { return static_cast<int64_t>(x) * x + static_cast<int64_t>(y) * y; }

    /**
     * @brief Dot product (scalar result)
     * @param rhs Right-hand vector
     * @return Dot product of this and rhs
     */
    constexpr int64_t Dot(const Vec2i& rhs) const noexcept { return static_cast<int64_t>(x) * rhs.x + static_cast<int64_t>(y) * rhs.y; }
    /**
     * @brief 2D cross product (scalar result)
     * @param rhs Right-hand vector
     * @return Scalar z of the 3D cross product (this x rhs)
     */
    constexpr int64_t Cross(const Vec2i& rhs) const noexcept { return static_cast<int64_t>(x) * rhs.y - static_cast<int64_t>(y) * rhs.x; }

//==============================================================================
// Static utilities
//==============================================================================

    /**
     * @brief Calculate distance between two points
     * @param a First point
     * @param b Second point
     * @return Euclidean distance between a and b
     */
    static float Distance(const Vec2i& a, const Vec2i& b) noexcept { return (b - a).Length(); }
    /**
     * @brief Calculate squared distance between two points (faster, avoids sqrt)
     * @param a First point
     * @param b Second point
     * @return Squared distance between a and b
     */
    static int64_t DistanceSquared(const Vec2i& a, const Vec2i& b) noexcept { return (b - a).LengthSquared(); }

//==============================================================================
// Common vectors
//==============================================================================

    static constexpr Vec2i Zero()  noexcept { return {0, 0}; }
    static constexpr Vec2i One()   noexcept { return {1, 1}; }
    static constexpr Vec2i UnitX() noexcept { return {1, 0}; }
    static constexpr Vec2i UnitY() noexcept { return {0, 1}; }
};

constexpr Vec2i operator*(int s, const Vec2i& v) noexcept { return v * s; }

inline std::ostream& operator<<(std::ostream& os, const Vec2i& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

} // namespace RS::Math
