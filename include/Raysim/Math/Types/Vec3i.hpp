#pragma once

#include "../Utils/MathUtils.hpp"

#include <cstdint>
#include <ostream>

namespace RS::Math {

/**
 * @struct Vec3i
 * @brief 3D vector with arithmetic and geometric operations
 *
 * Represents a 3D vector with integer components for positions, velocities, and directions in 3D space.
 * Includes 3D-specific operations like cross product with vector result and axis rotation.
 */
struct Vec3i {
    int x{0}, y{0}, z{0};

    constexpr Vec3i() noexcept = default;
    constexpr Vec3i(int in_x, int in_y, int in_z) noexcept : x(in_x), y(in_y), z(in_z) {}
    explicit constexpr Vec3i(int s) noexcept : x(s), y(s), z(s) {}

//==============================================================================
// Operators
//==============================================================================

    constexpr Vec3i operator+(const Vec3i& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
    constexpr Vec3i operator-(const Vec3i& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
    constexpr Vec3i operator*(int s)               const noexcept { return {x * s, y * s, z * s}; }
    constexpr Vec3i operator/(int s)               const noexcept { return (s != 0) ? Vec3i{x / s, y / s, z / s} : Vec3i{}; }

    constexpr Vec3i operator-() const noexcept { return {-x, -y, -z}; }

    constexpr Vec3i& operator+=(const Vec3i& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
    constexpr Vec3i& operator-=(const Vec3i& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
    constexpr Vec3i& operator*=(int s) noexcept               { x *= s; y *= s; z *= s; return *this; }
    constexpr Vec3i& operator/=(int s) noexcept               { if (s != 0) { x /= s; y /= s; z /= s; } return *this; }

    constexpr bool operator==(const Vec3i& o) const noexcept { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const Vec3i& o) const noexcept { return !(*this == o); }

    constexpr int& operator[](size_t i) noexcept { return (&x)[i]; }
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
    constexpr int64_t Dot(const Vec3i& rhs) const noexcept { return static_cast<int64_t>(x)*rhs.x + static_cast<int64_t>(y)*rhs.y + static_cast<int64_t>(z)*rhs.z; }

    /**
     * @brief 3D cross product (vector result perpendicular to both)
     * @param rhs Right-hand vector
     * @return Cross product vector (this x rhs)
     */
    constexpr Vec3i Cross(const Vec3i& rhs) const noexcept {
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
    static float Distance(const Vec3i& a, const Vec3i& b) noexcept { return (b - a).Length(); }
    /**
     * @brief Calculate squared distance between two points (faster, avoids sqrt)
     * @param a First point
     * @param b Second point
     * @return Squared distance between a and b
     */
    static int64_t DistanceSquared(const Vec3i& a, const Vec3i& b) noexcept { return (b - a).LengthSquared(); }

//==============================================================================
// Common vectors
//==============================================================================

    static constexpr Vec3i Zero()  noexcept { return {0, 0, 0}; }
    static constexpr Vec3i One()   noexcept { return {1, 1, 1}; }
    static constexpr Vec3i UnitX() noexcept { return {1, 0, 0}; }
    static constexpr Vec3i UnitY() noexcept { return {0, 1, 0}; }
    static constexpr Vec3i UnitZ() noexcept { return {0, 0, 1}; }
};

constexpr Vec3i operator*(int s, const Vec3i& v) noexcept { return v * s; }

inline std::ostream& operator<<(std::ostream& os, const Vec3i& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

} // namespace RS::Math
