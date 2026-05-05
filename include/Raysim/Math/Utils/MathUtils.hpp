/**********************************************************************************************
 *   Raysim Math Utilities
 *
 *   This header provides common mathematical constants and utility functions such as clamping,
 *   interpolation, and angle conversions.  It is included by Math.hpp to re-export these
 *   functions as part of the public Math API.
 **********************************************************************************************/

#pragma once

#include <cmath>

namespace RS::Math {

//==============================================================================
// Constants
//==============================================================================

    /// Common mathematical constants
    constexpr float PI      = 3.14159265358979323846f;
    constexpr float TWO_PI  = 2.0f * PI;
    constexpr float HALF_PI = PI * 0.5f;
    constexpr float DEG2RAD = PI / 180.0f;
    constexpr float RAD2DEG = 180.0f / PI;

//==============================================================================
// Angle conversion
//==============================================================================

    /// Convert degrees to radians
    [[nodiscard]] constexpr float ToRadians(float degrees) { return degrees * DEG2RAD; }

    /// Convert radians to degrees
    [[nodiscard]] constexpr float ToDegrees(float radians) { return radians * RAD2DEG; }

//==============================================================================
// Basic helpers
//==============================================================================

    /// Clamp `value` to the inclusive range [`minValue`, `maxValue`].
    template <typename T>
    [[nodiscard]] constexpr T Clamp(T value, T minValue, T maxValue) noexcept
    {
        return (value < minValue) ? minValue : (value > maxValue) ? maxValue : value;
    }

    /// Clamp `value` to the range [0, 1].
    [[nodiscard]] constexpr float Clamp01(float value) noexcept { return Clamp(value, 0.0f, 1.0f); }

    /// Return the minimum of two values.
    template <typename T>
    [[nodiscard]] constexpr T MinValue(T a, T b) noexcept
    {
        return (a < b) ? a : b;
    }

    /// Return the maximum of two values.
    template <typename T>
    [[nodiscard]] constexpr T MaxValue(T a, T b) noexcept
    {
        return (a > b) ? a : b;
    }

    /// Return the sign of `value` as -1, 0, or 1.
    template <typename T>
    [[nodiscard]] constexpr int Sign(T value) noexcept
    {
        return (T{} < value) - (value < T{});
    }

//==============================================================================
// Interpolation and remapping
//==============================================================================

    /**
     * Linearly interpolate between `a` and `b`.
     *
     * The interpolation factor `t` is clamped to [0, 1].
     */
    template <typename T>
    [[nodiscard]] constexpr T Lerp(T a, T b, float t) noexcept
    {
        t = Clamp01(t);
        return static_cast<T>(a + (b - a) * t);
    }

    /// Linearly interpolate between `a` and `b` without clamping `t`.
    template <typename T>
    [[nodiscard]] constexpr T LerpUnclamped(T a, T b, float t) noexcept
    {
        return static_cast<T>(a + (b - a) * t);
    }

    /**
     * Inverse linear interpolation.
     *
     * Returns `t` such that `LerpUnclamped(a, b, t) == value`. If `a` and `b` are
     * too close, returns 0.
     */
    [[nodiscard]] inline float InverseLerp(float a, float b, float value) noexcept
    {
        const float denom = (b - a);
        return (std::fabs(denom) <= 1e-12f) ? 0.0f : Clamp01((value - a) / denom);
    }

    /// Remap `value` from [inMin, inMax] to [outMin, outMax] (clamped).
    [[nodiscard]] inline float Remap(
        float value,
        float inMin,
        float inMax,
        float outMin,
        float outMax) noexcept
    {
        const float t = InverseLerp(inMin, inMax, value);
        return LerpUnclamped(outMin, outMax, t);
    }

//==============================================================================
// Stepping / periodic functions
//==============================================================================

    /// Move `current` towards `target` by at most `maxDelta`.
    [[nodiscard]] inline float MoveTowards(float current, float target, float maxDelta) noexcept
    {
        if (std::fabs(target - current) <= maxDelta) return target;
        return current + (target > current ? maxDelta : -maxDelta);
    }

    /// Repeat `t` so that it is in the range [0, length).
    [[nodiscard]] inline float Repeat(float t, float length) noexcept
    {
        if (length <= 0.0f) return 0.0f;
        return t - std::floor(t / length) * length;
    }

    /// Ping-pong `t` so that it moves back and forth between 0 and `length`.
    [[nodiscard]] inline float PingPong(float t, float length) noexcept
    {
        const float r = Repeat(t, length * 2.0f);
        return length - std::fabs(r - length);
    }

    /// Smoothly interpolate between 0 and 1 as `t` goes from 0 to 1.
    [[nodiscard]] inline float SmoothStep01(float t) noexcept
    {
        t = Clamp01(t);
        return t * t * (3.0f - 2.0f * t);
    }

} // namespace RS::Math
