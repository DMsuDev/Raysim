#pragma once

#include <cmath>
#include <random>
#include <algorithm>

namespace RS {

// ==========================
// Vec2: 2D vector class
// ==========================
struct Vec2 {
    float x{0.0f}, y{0.0f};

    // --------------------------
    // Constructors
    // --------------------------
    Vec2() = default;
    Vec2(float xx, float yy) : x(xx), y(yy) {}
    explicit Vec2(float s) : x(s), y(s) {}

    Vec2& operator=(float s) { x = y = s; return *this; }

    // --------------------------
    // Arithmetic operators
    // --------------------------
    Vec2  operator+(const Vec2& o) const { return {x + o.x, y + o.y}; }
    Vec2  operator-(const Vec2& o) const { return {x - o.x, y - o.y}; }
    Vec2  operator*(float s) const { return {x * s, y * s}; }
    Vec2  operator/(float s) const { return (s != 0.0f) ? (*this * (1.0f/s)) : Vec2{}; }

    Vec2& operator+=(const Vec2& o) { x += o.x; y += o.y; return *this; }
    Vec2& operator-=(const Vec2& o) { x -= o.x; y -= o.y; return *this; }
    Vec2& operator*=(float s) { x *= s; y *= s; return *this; }
    Vec2& operator/=(float s) { if (s != 0.0f) *this *= (1.0f/s); return *this; }

    bool operator==(const Vec2& o) const { return x == o.x && y == o.y; }
    bool operator!=(const Vec2& o) const { return !(*this == o); }

    // --------------------------
    // Vector properties
    // --------------------------
    float mag() const { return std::hypot(x, y); }        // magnitude
    float magSq() const { return x*x + y*y; }            // squared magnitude
    float dot(const Vec2& o) const { return x*o.x + y*o.y; }
    float cross(const Vec2& o) const { return x*o.y - y*o.x; } // scalar in 2D

    Vec2 normalized() const {
        float m = mag();
        return (m > 1e-6f) ? *this / m : Vec2{};
    }
    void normalize() { float m = mag(); if (m > 1e-6f) *this /= m; }

    void limit(float max) {
        float m2 = magSq();
        if (m2 > max*max) { normalize(); *this *= max; }
    }
    void setMag(float m) { normalize(); *this *= m; }

    float heading() const { return std::atan2(y, x); }

    void rotate(float angle) {
        float c = std::cos(angle), s = std::sin(angle);
        float nx = x*c - y*s;
        float ny = x*s + y*c;
        x = nx; y = ny;
    }

    // --------------------------
    // Static utility functions
    // --------------------------
    static float dist(const Vec2& a, const Vec2& b) { return (b - a).mag(); }
    static Vec2 lerp(const Vec2& a, const Vec2& b, float t) {
        t = std::clamp(t, 0.0f, 1.0f);
        return a + (b - a) * t;
    }
    static Vec2 fromAngle(float angle) { return {std::cos(angle), std::sin(angle)}; }

    // random unit vector in 2D
    static Vec2 random2D(std::mt19937& rng) {
        static constexpr float TAU = 6.28318530718f;
        std::uniform_real_distribution<float> dist(0.0f, TAU);
        return fromAngle(dist(rng));
    }

    // Get integer pixel coordinates (for drawing)
    int ix() const { return static_cast<int>(std::round(x)); }
    int iy() const { return static_cast<int>(std::round(y)); }
};

// free function for scalar multiplication from left
inline Vec2 operator*(float s, const Vec2& v) { return v * s; }

} // namespace RS
