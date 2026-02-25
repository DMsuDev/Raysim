#pragma once

#include "Raysim/Math/Math.hpp"

namespace RS {

/**
 * @struct Vector3
 * @brief 3D vector with arithmetic and geometric operations
 *
 * Represents a 3D vector for positions, velocities, and directions in 3D space.
 * Includes 3D-specific operations like cross product with vector result and axis rotation.
 */
struct Vector3 {
    float x{0.0f}, y{0.0f}, z{0.0f};  ///< X, Y, and Z components

    Vector3() = default;
    /// @brief Construct from x, y, z components
    Vector3(float x, float y, float z) : x(x), y(y), z(z) {}
    /// @brief Construct uniform vector (x = y = z = s)
    explicit Vector3(float s) : x(s), y(s), z(s) {}

    // ==== Arithmetic operators ====

    /// @brief Vector addition
    Vector3 operator+(const Vector3& o) const noexcept { return {x + o.x, y + o.y, z + o.z}; }
    /// @brief Vector subtraction
    Vector3 operator-(const Vector3& o) const noexcept { return {x - o.x, y - o.y, z - o.z}; }
    /// @brief Scalar multiplication
    Vector3 operator*(float s)          const noexcept { return {x * s, y * s, z * s}; }
    /// @brief Scalar division
    Vector3 operator/(float s)          const noexcept { return (s != 0.0f) ? (*this * (1.0f/s)) : Vector3{}; }

    /// @brief Negation
    Vector3 operator-() const noexcept { return {-x, -y, -z}; }

    Vector3& operator+=(const Vector3& o) { x += o.x; y += o.y; z += o.z; return *this; }
    Vector3& operator-=(const Vector3& o) { x -= o.x; y -= o.y; z -= o.z; return *this; }
    Vector3& operator*=(float s)          { x *= s; y *= s; z *= s; return *this; }
    Vector3& operator/=(float s)          { if (s != 0.0f) *this *= (1.0f/s); return *this; }

    bool operator==(const Vector3& o) const { return x == o.x && y == o.y && z == o.z; }
    bool operator!=(const Vector3& o) const { return !(*this == o); }

    // ==== Math operations ====

    /// @brief Calculate vector magnitude (length)
    float Length() const { return std::hypot(std::hypot(x, y), z); }
    /// @brief Calculate squared magnitude (faster, avoids sqrt)
    float LengthSquared() const { return x*x + y*y + z*z; }

    /// @brief Dot product (scalar result)
    float Dot(const Vector3& rhs) const { return x*rhs.x + y*rhs.y + z*rhs.z; }

    /// @brief 3D cross product (vector result perpendicular to both)
    Vector3 Cross(const Vector3& rhs) const {
        return {
            y * rhs.z - z * rhs.y,
            z * rhs.x - x * rhs.z,
            x * rhs.y - y * rhs.x
        };
    }

    /// @brief Limit magnitude to maximum value
    void Limit(float max) {
        float m2 = LengthSquared();
        if (m2 > max * max) {
            float m = std::sqrt(m2);
            *this *= (max / m);
        }
    }

    /// @brief Set magnitude to specific value
    void SetMag(float m) {
        float len = Length();
        if (len > 1e-6f) {
            *this = (*this / len) * m;
        }
    }

    /// @brief Return normalized vector
    Vector3 Normalized() const {
        float m = Length();
        return (m > 1e-6f) ? *this / m : Vector3{};
    }

    /// @brief Normalize this vector in-place
    void Normalize() {
        float m = Length();
        if (m > 1e-6f) *this /= m;
    }

    /// @brief Rotate around an axis using Rodrigues' rotation formula
    /// @param axis Rotation axis (normalized automatically)
    /// @param angle Rotation angle in radians
    Vector3 Rotate(const Vector3& axis, float angle) const {
        Vector3 k = axis.Normalized();
        float c = std::cos(angle);
        float s = std::sin(angle);

        return *this * c +
               k.Cross(*this) * s +
               k * (k.Dot(*this) * (1.0f - c));
    }

    /// @brief Project this vector onto another
    Vector3 Project(const Vector3& onto) const {
        float d = onto.LengthSquared();
        return (d > 1e-6f) ? onto * (Dot(onto) / d) : Vector3{};
    }

    /// @brief Reflect this vector across a surface normal
    Vector3 Reflect(const Vector3& n) const {
        return *this - n * (2.0f * Dot(n));
    }

    /// @brief Calculate angle in XZ plane (horizontal rotation)
    float HeadingXZ() const { return std::atan2(z, x); }
    /// @brief Calculate angle in XY plane
    float HeadingXY() const { return std::atan2(y, x); }

    // ==== Static utility functions ====

    /// @brief Calculate distance between two points
    static float Distance(const Vector3& a, const Vector3& b) { return (b - a).Length(); }

    /// @brief Create unit vector from angle in XY plane
    static Vector3 FromAngleXY(float angle) { return {std::cos(angle), std::sin(angle), 0.0f}; }
    /// @brief Create unit vector from angle in XZ plane
    static Vector3 FromAngleXZ(float angle) { return {std::cos(angle), 0.0f, std::sin(angle)}; }

    static float AngleBetween(const Vector3& a, const Vector3& b) {
        float dot = a.Dot(b);
        float m = std::sqrt(a.LengthSquared() * b.LengthSquared());
        if (m < 1e-6f) return 0.0f;
        return std::acos(dot / m);
    }

    // Common constants
    static Vector3 Zero()  { return {0.0f, 0.0f, 0.0f}; }
    static Vector3 One()   { return {1.0f, 1.0f, 1.0f}; }
    static Vector3 UnitX() { return {1.0f, 0.0f, 0.0f}; }
    static Vector3 UnitY() { return {0.0f, 1.0f, 0.0f}; }
    static Vector3 UnitZ() { return {0.0f, 0.0f, 1.0f}; }

    // Pixel coordinates
    int ix() const { return static_cast<int>(std::round(x)); }
    int iy() const { return static_cast<int>(std::round(y)); }
    int iz() const { return static_cast<int>(std::round(z)); }
};

} // namespace RS
