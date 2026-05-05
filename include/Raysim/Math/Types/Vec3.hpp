#pragma once

#include "../Utils/MathUtils.hpp"

#include <cstddef>
#include <ostream>

namespace RS::Math {

/**
 * @struct Vec3
 * @brief 3D vector with arithmetic and geometric operations
 *
 * Represents a 3D vector for positions, velocities, and directions in 3D space.
 * Includes 3D-specific operations like cross product with vector result and axis rotation.
 */
struct Vec3 {
    float x{0.0f}, y{0.0f}, z{0.0f};

    /**
     * @brief Default construct (zero vector)
     */
    constexpr Vec3() noexcept = default;
    /**
     * @brief Construct from x, y, z components
     * @param in_x X component
     * @param in_y Y component
     * @param in_z Z component
     */
    constexpr Vec3(float in_x, float in_y, float in_z) noexcept : x(in_x), y(in_y), z(in_z) {}
    /**
     * @brief Construct uniform vector (x = y = z = s)
     * @param s Scalar applied to all components
     */
    explicit constexpr Vec3(float s) noexcept : x(s), y(s), z(s) {}

//==============================================================================
// Operators
//==============================================================================

    constexpr Vec3 operator+(const Vec3& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
    constexpr Vec3 operator-(const Vec3& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
    constexpr Vec3 operator*(float s)          const noexcept { return {x * s, y * s, z * s}; }
    constexpr Vec3 operator/(float s)          const noexcept { return (s != 0.0f && s == s) ? (*this * (1.0f/s)) : Vec3{}; }

    constexpr Vec3 operator-() const noexcept { return {-x, -y, -z}; }

    constexpr Vec3& operator+=(const Vec3& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
    constexpr Vec3& operator-=(const Vec3& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
    constexpr Vec3& operator*=(float s) noexcept          { x *= s; y *= s; z *= s; return *this; }
    constexpr Vec3& operator/=(float s) noexcept          { if (s != 0.0f && s == s) *this *= (1.0f/s); return *this; }

    constexpr bool operator==(const Vec3& o) const noexcept { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const Vec3& o) const noexcept { return !(*this == o); }

    constexpr float& operator[](size_t i) noexcept { return (&x)[i]; }
    constexpr const float& operator[](size_t i) const noexcept { return (&x)[i]; }

//==============================================================================
// Math
//==============================================================================

    /**
     * @brief Calculate vector magnitude (length)
     * @return Euclidean length of the vector
     */
    float Length() const noexcept { return std::hypot(std::hypot(x, y), z); }
    /**
     * @brief Calculate squared magnitude (faster, avoids sqrt)
     * @return Squared length
     */
    constexpr float LengthSquared() const noexcept { return x*x + y*y + z*z; }

    /**
     * @brief Dot product (scalar result)
     * @param rhs Right-hand vector
     * @return Dot product
     */
    constexpr float Dot(const Vec3& rhs) const noexcept { return x*rhs.x + y*rhs.y + z*rhs.z; }

    /**
     * @brief 3D cross product (vector result perpendicular to both)
     * @param rhs Right-hand vector
     * @return Cross product vector (this x rhs)
     */
    constexpr Vec3 Cross(const Vec3& rhs) const noexcept {
        return {
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        };
    }

    /**
     * @brief Limit magnitude to maximum value
     * @param max Maximum magnitude allowed
     */
    void Limit(float max) noexcept {
        float m2 = LengthSquared();
        if (m2 > max * max) {
            float m = std::sqrt(m2);
            *this *= (max / m);
        }
    }

    /**
     * @brief Set magnitude to specific value (preserve direction)
     * @param m Target magnitude
     */
    void SetMag(float m) noexcept {
        float len = Length();
        if (len > 1e-6f) {
            *this = (*this / len) * m;
        }
    }

    /**
     * @brief Return normalized vector
     * @return Unit vector in same direction or zero vector if magnitude ~0
     */
    Vec3 Normalized() const noexcept {
        float m = Length();
        return (m > 1e-6f) ? *this / m : Vec3{};
    }

    /**
     * @brief Normalize this vector in-place
     */
    void Normalize() noexcept {
        float m = Length();
        if (m > 1e-6f) *this /= m;
    }

    /**
     * @brief Rotate around an axis using Rodrigues' rotation formula
     * @param axis Rotation axis (normalized automatically)
     * @param angle Rotation angle in radians
     */
    Vec3 Rotate(const Vec3& axis, float angle) const noexcept {
        Vec3 k = axis.Normalized();
        float c = std::cos(angle);
        float s = std::sin(angle);

        return *this * c +
               k.Cross(*this) * s +
               k * (k.Dot(*this) * (1.0f - c));
    }

    /**
     * @brief Project this vector onto another
     * @param onto Vector to project onto
     * @return Projection of this onto `onto`
     */
    constexpr Vec3 Project(const Vec3& onto) const noexcept {
        float d = onto.LengthSquared();
        return (d > 1e-6f) ? onto * (Dot(onto) / d) : Vec3{};
    }

    /**
     * @brief Reflect this vector across a surface normal
     * @param n Surface normal (should be normalized)
     * @return Reflected vector
     */
    constexpr Vec3 Reflect(const Vec3& n) const noexcept {
        return *this - n * (2.0f * Dot(n));
    }

    /**
     * @brief Calculate angle in XZ plane (horizontal rotation)
     * @return Angle in radians
     */
    float HeadingXZ() const noexcept { return std::atan2(z, x); }
    /**
     * @brief Calculate angle in XY plane
     * @return Angle in radians
     */
    float HeadingXY() const noexcept { return std::atan2(y, x); }


//==============================================================================
// Static utilities
//==============================================================================

    /**
     * @brief Calculate distance between two points
     * @param a First point
     * @param b Second point
     * @return Euclidean distance between a and b
     */
    static float Distance(const Vec3& a, const Vec3& b) noexcept { return (b - a).Length(); }
    /**
     * @brief Calculate squared distance between two points (faster, avoids sqrt)
     * @param a First point
     * @param b Second point
     * @return Squared distance between a and b
     */
    static float DistanceSquared(const Vec3& a, const Vec3& b) noexcept { return (b - a).LengthSquared(); }

    /**
     * @brief Create unit vector from angle in XY plane
     * @param angle Angle in radians
     * @return Unit Vec3 on XY plane
     */
    static Vec3 FromAngleXY(float angle) noexcept { return {std::cos(angle), std::sin(angle), 0.0f}; }
    /**
     * @brief Create unit vector from angle in XZ plane
     * @param angle Angle in radians
     * @return Unit Vec3 on XZ plane
     */
    static Vec3 FromAngleXZ(float angle) noexcept { return {std::cos(angle), 0.0f, std::sin(angle)}; }

    /**
     * @brief Angle between two vectors (0..pi)
     * @param a First vector
     * @param b Second vector
     * @return Angle in radians between a and b
     */
    static float AngleBetween(const Vec3& a, const Vec3& b) noexcept {
        float dot = a.Dot(b);
        float m = std::sqrt(a.LengthSquared() * b.LengthSquared());
        if (m < 1e-6f) return 0.0f;
        return std::acos(dot / m);
    }


//==============================================================================
// Common vectors
//==============================================================================

    static constexpr Vec3 Zero() noexcept  { return {0.0f, 0.0f, 0.0f}; }
    static constexpr Vec3 One() noexcept   { return {1.0f, 1.0f, 1.0f}; }
    static constexpr Vec3 UnitX() noexcept { return {1.0f, 0.0f, 0.0f}; }
    static constexpr Vec3 UnitY() noexcept { return {0.0f, 1.0f, 0.0f}; }
    static constexpr Vec3 UnitZ() noexcept { return {0.0f, 0.0f, 1.0f}; }
};

constexpr Vec3 operator*(float s, const Vec3& v) noexcept { return v * s; }

inline std::ostream& operator<<(std::ostream& os, const Vec3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

} // namespace RS::Math
