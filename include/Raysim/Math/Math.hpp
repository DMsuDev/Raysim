#pragma once

#ifdef PI
    #undef PI
#endif
#ifdef TWO_PI
    #undef TWO_PI
#endif

#include <cmath>

namespace RS {

/**
 * @namespace Math
 * @brief Common mathematical constants and utility functions
 */
namespace Math {
    /// Common mathematical constants
    constexpr float PI      = 3.14159265358979323846f;
    constexpr float TWO_PI  = 2.0f * PI;
    constexpr float HALF_PI = PI / 2.0f;
    constexpr float DEG2RAD = PI / 180.0f;
    constexpr float RAD2DEG = 180.0f / PI;

    /// @brief Convert degrees to radians
    constexpr float ToRadians(float degrees) { return degrees * DEG2RAD; }

    /// @brief Convert radians to degrees
    constexpr float ToDegrees(float radians) { return radians * RAD2DEG; }

    /// @brief Clamp value between min and max
    template<typename T>
    constexpr T Clamp(T value, T minVal, T maxVal) {
        return (value < minVal) ? minVal : (value > maxVal) ? maxVal : value;
    }

    /// @brief Linear interpolate between a and b (t clamped to 0-1)
    template<typename T>
    constexpr T Lerp(T a, T b, float t) {
        t = Clamp(t, 0.0f, 1.0f);
        return static_cast<T>(a + (b - a) * t);
    }

    /// @brief Return minimum of two values
    template<typename T>
    constexpr T Min(T a, T b) { return (a < b) ? a : b; }

    /// @brief Return maximum of two values
    template<typename T>
    constexpr T Max(T a, T b) { return (a > b) ? a : b; }

} // namespace Math

} // namespace RS
