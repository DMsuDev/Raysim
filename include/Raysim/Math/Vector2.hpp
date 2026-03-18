#pragma once

#include "Math.hpp"

#include <cstddef>

namespace RS {

/**
 * @struct Vector2
 * @brief 2D vector with arithmetic and geometric operations
 *
 * Represents a 2D vector with floating-point components for positions, velocities,
 * and directions. Includes common operations like dot product, normalization, and projection.
 */
struct Vector2 {
    float x{0.0f}, y{0.0f};

    /**
     * @brief Default construct (zero vector)
     */
    constexpr Vector2() noexcept = default;
    /**
     * @brief Construct from x and y components
     * @param x X component
     * @param y Y component
     */
    constexpr Vector2(float x, float y) noexcept : x(x), y(y) {}
    /**
     * @brief Construct uniform vector (x = y = s)
     * @param s Scalar applied to both components
     */
    explicit constexpr Vector2(float s) noexcept : x(s), y(s) {}

//==============================================================================
// Operators
//==============================================================================

    constexpr Vector2 operator+(const Vector2& o) const noexcept { return {x + o.x, y + o.y}; }
    constexpr Vector2 operator-(const Vector2& o) const noexcept { return {x - o.x, y - o.y}; }
    constexpr Vector2 operator*(float s)          const noexcept { return {x * s, y * s}; }
    constexpr Vector2 operator/(float s)          const noexcept { return (s != 0.0f) ? (*this * (1.0f/s)) : Vector2{}; }

    constexpr Vector2 operator-() const noexcept { return {-x, -y}; }

    constexpr Vector2& operator+=(const Vector2& o) noexcept { x += o.x; y += o.y; return *this; }
    constexpr Vector2& operator-=(const Vector2& o) noexcept { x -= o.x; y -= o.y; return *this; }
    constexpr Vector2& operator*=(float s) noexcept { x *= s; y *= s; return *this; }
    constexpr Vector2& operator/=(float s) noexcept { if (s != 0.0f) *this *= (1.0f/s); return *this; }

    constexpr bool operator==(const Vector2& o) const noexcept { return x == o.x && y == o.y; }
    constexpr bool operator!=(const Vector2& o) const noexcept { return !(*this == o); }

    /**
     * @brief Access vector components by index (non-const)
     * @param i Index (0 = x, 1 = y)
     * @return Reference to the referenced component
     */
    constexpr float& operator[](size_t i) noexcept{ return (&x)[i]; }

    /**
     * @brief Access vector components by index (const)
     * @param i Index (0 = x, 1 = y)
     * @return Const reference to the component
     */
    constexpr const float& operator[](size_t i) const noexcept{ return (&x)[i]; }

//==============================================================================
// Math
//==============================================================================

    /**
     * @brief Calculate vector magnitude (length)
     * @return Euclidean length of the vector
     */
    float Length() const noexcept { return std::hypot(x, y); }
    /**
     * @brief Calculate squared magnitude (faster, avoids sqrt)
     * @return Squared length of the vector
     */
    constexpr float LengthSquared() const noexcept { return x * x + y * y; }

    /**
     * @brief Dot product (scalar result)
     * @param rhs Right-hand vector
     * @return Dot product of this and rhs
     */
    constexpr float Dot(const Vector2& rhs) const noexcept { return x * rhs.x + y * rhs.y; }
    /**
     * @brief 2D cross product (scalar result)
     * @param rhs Right-hand vector
     * @return Scalar z of the 3D cross product (this x rhs)
     */
    constexpr float Cross(const Vector2& rhs) const noexcept { return x * rhs.y - y * rhs.x; }

    /**
     * @brief Limit magnitude to maximum value
     * @param max Maximum allowed magnitude
     */
    void Limit(float max) noexcept
    {
        float m2 = LengthSquared();
        if (m2 > max * max) {
            float m = std::sqrt(m2);
            *this *= (max / m);
        }
    }
    /**
     * @brief Set magnitude to specific value (preserves direction)
     * @param m New magnitude to set
     */
    void SetMag(float m) noexcept
    {
        float len = Length();
        if (len > 1e-6f) {
            *this = (*this / len) * m;
        }
    }

    /**
     * @brief Calculate angle from vector to positive x-axis (radians)
     * @return Angle in radians in range (-pi, pi]
     */
    float Heading() const noexcept { return std::atan2(y, x); }

    /**
     * @brief Perpendicular vector (rotated 90° counterclockwise)
     * @return Perpendicular vector
     */
    constexpr Vector2 Perp() const noexcept { return {-y, x}; }

    /**
     * @brief Return normalized vector
     * @return Unit vector in the same direction or zero vector if length is ~0
     */
    Vector2 Normalized() const noexcept
    {
        float m = Length();
        return (m > 1e-6f) ? *this / m : Vector2{};
    }

    /**
     * @brief Normalize this vector in-place, returns false if length is too small
     * @return true if normalized, false if length was too small
     */
    bool NormalizeSafe() noexcept
    {
        float len = Length();
        if (len <= 1e-6f) return false;
        *this /= len;
        return true;
    }

    /**
     * @brief Normalize this vector in-place (no return)
     */
    void Normalize() noexcept { float m = Length(); if (m > 1e-6f) *this /= m; }

    /**
     * @brief Rotate by angle (radians)
     * @param angle Rotation angle in radians (counterclockwise)
     * @return Rotated vector
     */
    Vector2 Rotate(float angle) const noexcept
    {
        float c = std::cos(angle), s = std::sin(angle);
        float nx = x*c - y*s;
        float ny = x*s + y*c;
        return {nx, ny};
    }
    /**
     * @brief Project this vector onto another
     * @param onto Vector to project onto
     * @return Projection of this onto `onto`
     */
    constexpr Vector2 Project(const Vector2& onto) const noexcept
    {
        float d = onto.LengthSquared();
        return (d > 1e-6f) ? onto * (Dot(onto) / d) : Vector2{};
    }
    /**
     * @brief Reflect this vector across a surface normal
     * @param n Surface normal (should be normalized)
     * @return Reflected vector
     */
    constexpr Vector2 Reflect(const Vector2& n) const noexcept
    {
        return *this - n * (2.0f * Dot(n));
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
    static float Distance(const Vector2& a, const Vector2& b) noexcept { return (b - a).Length(); }
    /**
     * @brief Calculate squared distance between two points (faster, avoids sqrt)
     * @param a First point
     * @param b Second point
     * @return Squared distance between a and b
     */
    static float DistanceSquared(const Vector2& a, const Vector2& b) noexcept { return (b - a).LengthSquared(); }

    /**
     * @brief Create unit vector from angle
     * @param angle Angle in radians from positive x-axis (counterclockwise)
     * @return Unit vector at the specified angle
     */
    static Vector2 FromAngle(float angle) noexcept { return {std::cos(angle), std::sin(angle)}; }

    /**
     * @brief Angle between two vectors (0..pi)
     * @param a First vector
     * @param b Second vector
     * @return Angle in radians between a and b
     */
    static float AngleBetween(const Vector2& a, const Vector2& b) noexcept {
        float dot = a.Dot(b);
        float mag = std::sqrt(a.LengthSquared() * b.LengthSquared());
        if (mag < 1e-6f) return 0.0f;
        return std::acos(dot / mag);
    }

//==============================================================================
// Common vectors
//==============================================================================

    static constexpr Vector2 Zero() noexcept  { return {0.0f, 0.0f}; }
    static constexpr Vector2 One() noexcept   { return {1.0f, 1.0f}; }
    static constexpr Vector2 UnitX() noexcept { return {1.0f, 0.0f}; }
    static constexpr Vector2 UnitY() noexcept { return {0.0f, 1.0f}; }

    /// Rounded x coordinate as integer (useful for pixel-space operations).
    int ix() const noexcept { return static_cast<int>(std::round(x)); }

    /// Rounded y coordinate as integer (useful for pixel-space operations).
    int iy() const noexcept { return static_cast<int>(std::round(y)); }
};

constexpr Vector2 operator*(float s, const Vector2& v) noexcept { return v * s; }

} // namespace RS
