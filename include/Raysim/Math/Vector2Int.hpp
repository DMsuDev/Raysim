#pragma once

#include "Math.hpp"

#include <cstddef>
#include <ostream>

namespace RS {

/**
 * @struct Vector2Int
 * @brief 2D vector with arithmetic and geometric operations
 *
 * Represents a 2D vector with integer components for positions, velocities,
 * and directions. Includes common operations like dot product, normalization, and projection.
 */
struct Vector2Int {
    int x{0}, y{0};

    /**
     * @brief Default construct (zero vector)
     */
    constexpr Vector2Int() noexcept = default;
    /**
     * @brief Construct from x and y components
     * @param in_x X component
     * @param in_y Y component
     */
    constexpr Vector2Int(int in_x, int in_y) noexcept : x(in_x), y(in_y) {}
    /**
     * @brief Construct uniform vector (x = y = s)
     * @param s Scalar applied to both components
     */
    explicit constexpr Vector2Int(int s) noexcept : x(s), y(s) {}

//==============================================================================
// Operators
//==============================================================================

    constexpr Vector2Int operator+(const Vector2Int& o) const noexcept { return {x + o.x, y + o.y}; }
    constexpr Vector2Int operator-(const Vector2Int& o) const noexcept { return {x - o.x, y - o.y}; }
    constexpr Vector2Int operator*(int s)               const noexcept { return {x * s, y * s}; }
    constexpr Vector2Int operator/(int s)               const noexcept { return (s != 0) ? Vector2Int{x / s, y / s} : Vector2Int{}; }

    constexpr Vector2Int operator-() const noexcept { return {-x, -y}; }

    constexpr Vector2Int& operator+=(const Vector2Int& o) noexcept { x += o.x; y += o.y; return *this; }
    constexpr Vector2Int& operator-=(const Vector2Int& o) noexcept { x -= o.x; y -= o.y; return *this; }
    constexpr Vector2Int& operator*=(int s)               noexcept { x *= s; y *= s; return *this; }
    constexpr Vector2Int& operator/=(int s)               noexcept { if (s != 0) { x /= s; y /= s; } return *this; }

    constexpr bool operator==(const Vector2Int& o) const noexcept { return x == o.x && y == o.y; }
    constexpr bool operator!=(const Vector2Int& o) const noexcept { return !(*this == o); }

    /**
     * @brief Access vector components by index (non-const)
     * @param i Index (0 = x, 1 = y)
     * @return Reference to the referenced component
     */
    constexpr int& operator[](size_t i) noexcept{ return (&x)[i]; }

    /**
     * @brief Access vector components by index (const)
     * @param i Index (0 = x, 1 = y)
     * @return Const reference to the component
     */
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
    constexpr int64_t Dot(const Vector2Int& rhs) const noexcept { return static_cast<int64_t>(x) * rhs.x + static_cast<int64_t>(y) * rhs.y; }
    /**
     * @brief 2D cross product (scalar result)
     * @param rhs Right-hand vector
     * @return Scalar z of the 3D cross product (this x rhs)
     */
    constexpr int64_t Cross(const Vector2Int& rhs) const noexcept { return static_cast<int64_t>(x) * rhs.y - static_cast<int64_t>(y) * rhs.x; }

//==============================================================================
// Static utilities
//==============================================================================

    /**
     * @brief Calculate distance between two points
     * @param a First point
     * @param b Second point
     * @return Euclidean distance between a and b
     */
    static float Distance(const Vector2Int& a, const Vector2Int& b) noexcept { return (b - a).Length(); }
    /**
     * @brief Calculate squared distance between two points (faster, avoids sqrt)
     * @param a First point
     * @param b Second point
     * @return Squared distance between a and b
     */
    static int64_t DistanceSquared(const Vector2Int& a, const Vector2Int& b) noexcept { return (b - a).LengthSquared(); }

//==============================================================================
// Common vectors
//==============================================================================

    static constexpr Vector2Int Zero()  noexcept { return {0, 0}; }
    static constexpr Vector2Int One()   noexcept { return {1, 1}; }
    static constexpr Vector2Int UnitX() noexcept { return {1, 0}; }
    static constexpr Vector2Int UnitY() noexcept { return {0, 1}; }
};

constexpr Vector2Int operator*(int s, const Vector2Int& v) noexcept { return v * s; }

// ============================================================================
// Stream output operators
// ============================================================================

/**
 * @brief Stream output for Vector2Int
 * @param os Output stream
 * @param v Vector2Int to output
 * @return Reference to the output stream
 */
inline std::ostream& operator<<(std::ostream& os, const Vector2Int& v) {
    return os << "(" << v.x << ", " << v.y << ")";
}

} // namespace RS
