#pragma once

#include "Raysim/Math/MathUtils.hpp"

#include <cstdint>
#include <ostream>

namespace RS {

/**
 * @struct Vector3Int
 * @brief 3D vector with arithmetic and geometric operations
 *
 * Represents a 3D vector with integer components for positions, velocities, and directions in 3D space.
 * Includes 3D-specific operations like cross product with vector result and axis rotation.
 */
struct Vector3Int {
    int x{0}, y{0}, z{0};

    /**
     * @brief Default construct (zero vector)
     */
    constexpr Vector3Int() noexcept = default;
    /**
     * @brief Construct from x, y, z components
     * @param in_x X component
     * @param in_y Y component
     * @param in_z Z component
     */
    constexpr Vector3Int(int in_x, int in_y, int in_z) noexcept : x(in_x), y(in_y), z(in_z) {}
    /**
     * @brief Construct uniform vector (x = y = z = s)
     * @param s Scalar applied to all components
     */
    explicit constexpr Vector3Int(int s) noexcept : x(s), y(s), z(s) {}

//==============================================================================
// Operators
//==============================================================================

    constexpr Vector3Int operator+(const Vector3Int& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
    constexpr Vector3Int operator-(const Vector3Int& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
    constexpr Vector3Int operator*(int s)               const noexcept { return {x * s, y * s, z * s}; }
    constexpr Vector3Int operator/(int s)               const noexcept { return (s != 0) ? Vector3Int{x / s, y / s, z / s} : Vector3Int{}; }

    constexpr Vector3Int operator-() const noexcept { return {-x, -y, -z}; }

    constexpr Vector3Int& operator+=(const Vector3Int& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
    constexpr Vector3Int& operator-=(const Vector3Int& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
    constexpr Vector3Int& operator*=(int s) noexcept               { x *= s; y *= s; z *= s; return *this; }
    constexpr Vector3Int& operator/=(int s) noexcept               { if (s != 0) { x /= s; y /= s; z /= s; } return *this; }

    constexpr bool operator==(const Vector3Int& o) const noexcept { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const Vector3Int& o) const noexcept { return !(*this == o); }

    /**
     * @brief Access components by index (0=x,1=y,2=z)
     * @param i Index of component
     * @return Reference to component (non-const)
     */
    constexpr int& operator[](size_t i) noexcept { return (&x)[i]; }
    /**
     * @brief Const access to components by index (0=x,1=y,2=z)
     * @param i Index of component
     * @return Const reference to component
     */
    constexpr const int& operator[](size_t i) const noexcept { return (&x)[i]; }

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
     * @return Squared length
     */
    constexpr int64_t LengthSquared() const noexcept { return static_cast<int64_t>(x)*x + static_cast<int64_t>(y)*y + static_cast<int64_t>(z)*z; }

    /**
     * @brief Dot product (scalar result)
     * @param rhs Right-hand vector
     * @return Dot product
     */
    constexpr int64_t Dot(const Vector3Int& rhs) const noexcept { return static_cast<uint64_t>(x*rhs.x + y*rhs.y + z*rhs.z); }

    /**
     * @brief 3D cross product (vector result perpendicular to both)
     * @param rhs Right-hand vector
     * @return Cross product vector (this x rhs)
     */
    constexpr Vector3Int Cross(const Vector3Int& rhs) const noexcept {
        return {
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        };
    }

//==============================================================================
// Static utilities
//==============================================================================

    /**
     * @brief Calculate distance between two points
     * @param a First point
     * @param b Second point
     * @return Euclidean distance between a and b
     */
    static float Distance(const Vector3Int& a, const Vector3Int& b) noexcept { return (b - a).Length(); }
    /**
     * @brief Calculate squared distance between two points (faster, avoids sqrt)
     * @param a First point
     * @param b Second point
     * @return Squared distance between a and b
     */
    static int64_t DistanceSquared(const Vector3Int& a, const Vector3Int& b) noexcept { return (b - a).LengthSquared(); }

//==============================================================================
// Common vectors
//==============================================================================

    static constexpr Vector3Int Zero()  noexcept { return {0, 0, 0}; }
    static constexpr Vector3Int One()   noexcept { return {1, 1, 1}; }
    static constexpr Vector3Int UnitX() noexcept { return {1, 0, 0}; }
    static constexpr Vector3Int UnitY() noexcept { return {0, 1, 0}; }
    static constexpr Vector3Int UnitZ() noexcept { return {0, 0, 1}; }
};

constexpr Vector3Int operator*(int s, const Vector3Int& v) noexcept { return v * s; }

// ============================================================================
// Stream output operators
// ============================================================================

/**
 * @brief Stream output for Vector3Int
 * @param os Output stream
 * @param v Vector3Int to output
 * @return Reference to the output stream
 */
inline std::ostream& operator<<(std::ostream& os, const Vector3Int& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

} // namespace RS
