#pragma once

#include "Math.hpp"

#include <cstddef>
#include <ostream>

namespace RS {

/**
 * @struct Vector3
 * @brief 3D vector with arithmetic and geometric operations
 *
 * Represents a 3D vector for positions, velocities, and directions in 3D space.
 * Includes 3D-specific operations like cross product with vector result and axis rotation.
 */
struct Vector3 {
    float x{0.0f}, y{0.0f}, z{0.0f};

    /**
     * @brief Default construct (zero vector)
     */
    constexpr Vector3() noexcept = default;
    /**
     * @brief Construct from x, y, z components
     * @param x X component
     * @param y Y component
     * @param z Z component
     */
    constexpr Vector3(float x, float y, float z) noexcept : x(x), y(y), z(z) {}
    /**
     * @brief Construct uniform vector (x = y = z = s)
     * @param s Scalar applied to all components
     */
    explicit constexpr Vector3(float s) noexcept : x(s), y(s), z(s) {}

//==============================================================================
// Operators
//==============================================================================

    constexpr Vector3 operator+(const Vector3& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
    constexpr Vector3 operator-(const Vector3& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
    constexpr Vector3 operator*(float s)          const noexcept { return {x * s, y * s, z * s}; }
    constexpr Vector3 operator/(float s)          const noexcept { return (s != 0.0f && s == s) ? (*this * (1.0f/s)) : Vector3{}; }

    constexpr Vector3 operator-() const noexcept { return {-x, -y, -z}; }

    constexpr Vector3& operator+=(const Vector3& o) noexcept { x += o.x; y += o.y; z += o.z; return *this; }
    constexpr Vector3& operator-=(const Vector3& o) noexcept { x -= o.x; y -= o.y; z -= o.z; return *this; }
    constexpr Vector3& operator*=(float s) noexcept          { x *= s; y *= s; z *= s; return *this; }
    constexpr Vector3& operator/=(float s) noexcept          { if (s != 0.0f && s == s) *this *= (1.0f/s); return *this; }

    constexpr bool operator==(const Vector3& o) const noexcept { return x == o.x && y == o.y && z == o.z; }
    constexpr bool operator!=(const Vector3& o) const noexcept { return !(*this == o); }

    /**
     * @brief Access components by index (0=x,1=y,2=z)
     * @param i Index of component
     * @return Reference to component (non-const)
     */
    constexpr float& operator[](size_t i) noexcept { return (&x)[i]; }
    /**
     * @brief Const access to components by index (0=x,1=y,2=z)
     * @param i Index of component
     * @return Const reference to component
     */
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
    constexpr float Dot(const Vector3& rhs) const noexcept { return x*rhs.x + y*rhs.y + z*rhs.z; }

    /**
     * @brief 3D cross product (vector result perpendicular to both)
     * @param rhs Right-hand vector
     * @return Cross product vector (this x rhs)
     */
    constexpr Vector3 Cross(const Vector3& rhs) const noexcept {
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
    Vector3 Normalized() const noexcept {
        float m = Length();
        return (m > 1e-6f) ? *this / m : Vector3{};
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
    Vector3 Rotate(const Vector3& axis, float angle) const noexcept {
        Vector3 k = axis.Normalized();
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
    constexpr Vector3 Project(const Vector3& onto) const noexcept {
        float d = onto.LengthSquared();
        return (d > 1e-6f) ? onto * (Dot(onto) / d) : Vector3{};
    }

    /**
     * @brief Reflect this vector across a surface normal
     * @param n Surface normal (should be normalized)
     * @return Reflected vector
     */
    constexpr Vector3 Reflect(const Vector3& n) const noexcept {
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
    static float Distance(const Vector3& a, const Vector3& b) noexcept { return (b - a).Length(); }
    /**
     * @brief Calculate squared distance between two points (faster, avoids sqrt)
     * @param a First point
     * @param b Second point
     * @return Squared distance between a and b
     */
    static float DistanceSquared(const Vector3& a, const Vector3& b) noexcept { return (b - a).LengthSquared(); }

    /**
     * @brief Create unit vector from angle in XY plane
     * @param angle Angle in radians
     * @return Unit Vector3 on XY plane
     */
    static Vector3 FromAngleXY(float angle) noexcept { return {std::cos(angle), std::sin(angle), 0.0f}; }
    /**
     * @brief Create unit vector from angle in XZ plane
     * @param angle Angle in radians
     * @return Unit Vector3 on XZ plane
     */
    static Vector3 FromAngleXZ(float angle) noexcept { return {std::cos(angle), 0.0f, std::sin(angle)}; }

    /**
     * @brief Angle between two vectors (0..pi)
     * @param a First vector
     * @param b Second vector
     * @return Angle in radians between a and b
     */
    static float AngleBetween(const Vector3& a, const Vector3& b) noexcept {
        float dot = a.Dot(b);
        float m = std::sqrt(a.LengthSquared() * b.LengthSquared());
        if (m < 1e-6f) return 0.0f;
        return std::acos(dot / m);
    }


//==============================================================================
// Common vectors
//==============================================================================

    static constexpr Vector3 Zero() noexcept  { return {0.0f, 0.0f, 0.0f}; }
    static constexpr Vector3 One() noexcept   { return {1.0f, 1.0f, 1.0f}; }
    static constexpr Vector3 UnitX() noexcept { return {1.0f, 0.0f, 0.0f}; }
    static constexpr Vector3 UnitY() noexcept { return {0.0f, 1.0f, 0.0f}; }
    static constexpr Vector3 UnitZ() noexcept { return {0.0f, 0.0f, 1.0f}; }

    /// Rounded x coordinate as integer (useful for pixel-space operations).
    int ix() const noexcept { return static_cast<int>(std::round(x)); }

    /// Rounded y coordinate as integer (useful for pixel-space operations).
    int iy() const noexcept { return static_cast<int>(std::round(y)); }

    /// Rounded z coordinate as integer (useful for pixel-space operations).
    int iz() const noexcept { return static_cast<int>(std::round(z)); }
};

constexpr Vector3 operator*(float s, const Vector3& v) noexcept { return v * s; }

// ============================================================================
// Stream output operators
// ============================================================================

/**
 * @brief Stream output for Vector3
 * @param os Output stream
 * @param v Vector3 to output
 * @return Reference to the output stream
 */
inline std::ostream& operator<<(std::ostream& os, const Vector3& v) {
    return os << "(" << v.x << ", " << v.y << ", " << v.z << ")";
}

} // namespace RS
