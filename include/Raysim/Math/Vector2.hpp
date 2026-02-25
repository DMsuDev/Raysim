#pragma once

#include "Raysim/Math/Math.hpp"

namespace RS {

/**
 * @struct Vector2
 * @brief 2D vector with arithmetic and geometric operations
 *
 * Represents a 2D vector with floating-point components for positions, velocities,
 * and directions. Includes common operations like dot product, normalization, and projection.
 */
struct Vector2 {
    float x{0.0f}, y{0.0f};  ///< X and Y components

    Vector2() = default;
    /// @brief Construct from x and y components
    Vector2(float x, float y) : x(x), y(y) {}
    /// @brief Construct uniform vector (x = y = s)
    explicit Vector2(float s) : x(s), y(s) {}

    // ==== Arithmetic operators ====

    /// @brief Vector addition
    Vector2 operator+(const Vector2& o) const noexcept { return {x + o.x, y + o.y}; }
    /// @brief Vector subtraction
    Vector2 operator-(const Vector2& o) const noexcept { return {x - o.x, y - o.y}; }
    /// @brief Scalar multiplication
    Vector2 operator*(float s)          const noexcept { return {x * s, y * s}; }
    /// @brief Scalar division
    Vector2 operator/(float s)          const noexcept { return (s != 0.0f) ? (*this * (1.0f/s)) : Vector2{}; }

    /// @brief Negation
    Vector2 operator-() const noexcept { return {-x, -y}; }

    Vector2& operator+=(const Vector2& o) { x += o.x; y += o.y; return *this; }
    Vector2& operator-=(const Vector2& o) { x -= o.x; y -= o.y; return *this; }
    Vector2& operator*=(float s)          { x *= s; y *= s; return *this; }
    Vector2& operator/=(float s)          { if (s != 0.0f) *this *= (1.0f/s); return *this; }

    bool operator==(const Vector2& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Vector2& o) const { return !(*this == o); }

    // ==== Math operations ====

    /// @brief Calculate vector magnitude (length)
    float Length() const { return std::hypot(x, y); }
    /// @brief Calculate squared magnitude (faster, avoids sqrt)
    float LengthSquared() const { return x * x + y * y; }

    /// @brief Dot product (scalar result)
    float Dot(const Vector2& rhs) const { return x * rhs.x + y * rhs.y; }
    /// @brief 2D cross product (scalar result)
    float Cross(const Vector2& rhs) const { return x * rhs.y - y * rhs.x; }

    /// @brief Limit magnitude to maximum value
    void Limit(float max)
    {
        float m2 = LengthSquared();
        if (m2 > max * max) {
            float m = std::sqrt(m2);
            *this *= (max / m);
        }
    }
    /// @brief Set magnitude to specific value
    void SetMag(float m)
    {
        float len = Length();
        if (len > 1e-6f) {
            *this = (*this / len) * m;
        }
    }

    /// @brief Calculate angle from vector to positive x-axis (radians)
    float Heading() const { return std::atan2(y, x); }

    /// @brief Perpendicular vector (rotated 90° counterclockwise)
    Vector2 Perp() const noexcept { return {-y, x}; }

    /// @brief Return normalized vector
    Vector2 Normalized() const
    {
        float m = Length();
        return (m > 1e-6f) ? *this / m : Vector2{};
    }
    /// @brief Normalize this vector in-place
    void Normalize() { float m = Length(); if (m > 1e-6f) *this /= m; }

    /// @brief Rotate by angle (radians)
    Vector2 Rotate(float angle) const
    {
        float c = std::cos(angle), s = std::sin(angle);
        float nx = x*c - y*s;
        float ny = x*s + y*c;
        return {nx, ny};
    }
    /// @brief Project this vector onto another
    Vector2 Project(const Vector2& onto) const
    {
        float d = onto.LengthSquared();
        return (d > 1e-6f) ? onto * (Dot(onto) / d) : Vector2{};
    }
    /// @brief Reflect this vector across a surface normal
    Vector2 Reflect(const Vector2& n) const
    {
        return *this - n * (2.0f * Dot(n));
    }

    // ==== Static utility functions ====

    /// @brief Calculate distance between two points
    static float Distance(const Vector2& a, const Vector2& b) { return (b - a).Length(); }
    /// @brief Create unit vector from angle (radians)
    static Vector2 FromAngle(float angle) { return {std::cos(angle), std::sin(angle)}; }
    /// @brief Calculate angle between two vectors (returns 0 to 2π)
    static float AngleBetween(const Vector2& a, const Vector2& b)
    {
        float dot = a.Dot(b);
        float det = a.Cross(b);
        float angle = std::atan2(det, dot);
        if (angle < 0) { angle += 2 * Math::PI; }
        return angle;
    }

    // Common constants
    static Vector2 Zero()  { return {0.0f, 0.0f}; }
    static Vector2 One()   { return {1.0f, 1.0f}; }
    static Vector2 UnitX() { return {1.0f, 0.0f}; }
    static Vector2 UnitY() { return {0.0f, 1.0f}; }

    // Get integer pixel coordinates (for drawing)
    int ix() const { return static_cast<int>(std::round(x)); }
    int iy() const { return static_cast<int>(std::round(y)); }
};

} // namespace RS
